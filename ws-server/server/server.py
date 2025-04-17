import asyncio
import websockets
import json
import random
from datetime import datetime
from collections import defaultdict

class WebSocketServer:
    def __init__(self):
        self.clients = defaultdict(dict)  # {websocket: {"user_id": str, "room_id": str, "last_active": datetime}}
        self.user_connections = defaultdict(set)  # {user_id: set(websocket)}
        self.rooms = defaultdict(set)  # {room_id: set(websocket)}
        self.invites = {}  # {user_id: (inviter_id, room_id)}
        self.pending_deletion = {}  # {room_id: (task, timer)}


    async def broadcast(self, message_type: str, data: dict):
        """ 智能广播系统 """
        message = self._format_message(message_type, data)
        
        dead_connections = []
        for ws in list(self.clients.keys()):  # 避免遍历时修改
            try:
                await ws.send(message)
                self.clients[ws]["last_active"] = datetime.now()
            except websockets.exceptions.ConnectionClosed:
                dead_connections.append(ws)
        
        for ws in dead_connections:
            self._clean_connection(ws)

    def _format_message(self, message_type: str, data: dict):
        """ 格式化信息 """
        return json.dumps({
            "type": message_type,
            "data": data,
            "timestamp": datetime.now().isoformat()
        }, ensure_ascii=False)
        
    async def force_logout(self, user_id: str):
        """强制下线机制"""
        if user_id in self.user_connections:
            for ws in list(self.user_connections[user_id]):
                try:
                    await ws.send(json.dumps({
                        "type": "force_logout",
                        "reason": "重复登录",
                        "timestamp": datetime.now().isoformat()
                    }))
                except Exception as e:
                    print(f"[强制下线错误] {str(e)}")
                finally:
                    room_id = self.clients[ws].get("room_id")
                    if room_id:
                        await self.leave_room(ws, user_id, room_id)
                    await ws.close(code=4000, reason="Duplicate login")
                    self._clean_connection(ws)

            print(f"[{datetime.now()}] 用户 {user_id} 旧连接已清除")
            

    def _clean_connection(self, ws):
        """ 连接清理工具 """
        if ws in self.clients:
            user_id = self.clients[ws].get("user_id")
            if user_id:
                self.user_connections[user_id].discard(ws)
                if not self.user_connections[user_id]:
                    del self.user_connections[user_id]
            del self.clients[ws]

            
    async def connection_handler(self, websocket, path):
        """连接生命周期管理"""
        user_id = None
        try:
            # 认证阶段
            auth_timeout = 10
            try:
                user_id = await asyncio.wait_for(websocket.recv(), timeout=auth_timeout)
                user_id = user_id.strip()
            except asyncio.TimeoutError:
                await websocket.close(code=4001, reason="认证超时")
                return

            # 处理重复登录
            if user_id in self.user_connections:
                await self.force_logout(user_id)

            # 注册新连接
            self.clients[websocket] = {
                "user_id": user_id,
                "room_id": None,
                "last_active": datetime.now()
            }
            self.user_connections[user_id].add(websocket)
            print(f"[{datetime.now()}] 用户 {user_id} 已连接")

            # 新用户连接时广播
            await self.broadcast("join", {"user_id": user_id})
            

            # 接收消息循环
            async for message in websocket:
                if isinstance(message, bytes):
                    print("[收到二进制数据] 忽略")
                    continue
                    
                self.clients[websocket]["last_active"] = datetime.now()
                await self.handle_client_message(websocket, message)

        except websockets.exceptions.ConnectionClosedOK:
            print(f"[{datetime.now()}] 用户 {user_id} 正常断开")
        except Exception as e:
            print(f"[连接异常] {str(e)}")
            print(f"[{datetime.now()}] 用户 {user_id} 异常断开")
        finally:
            # 离开房间处理
            if user_id and websocket in self.clients:
                room_id = self.clients[websocket].get("room_id")
                if room_id:
                    await self.leave_room(websocket, user_id)
            
            self._clean_connection(websocket)
            if user_id:
                await self.broadcast("leave", {"user_id": user_id})

    async def connection_handler(self, websocket, path):
        """ 连接生命周期管理 """
        user_id = None
        try:
            # 认证阶段
            auth_timeout = 10
            try:
                user_id = await asyncio.wait_for(websocket.recv(), timeout=auth_timeout)
                user_id = user_id.strip()
            except asyncio.TimeoutError:
                await websocket.close(code=4001, reason="认证超时")
                return

            # 处理重复登录
            if user_id in self.user_connections:
                await self.force_logout(user_id)

            # 注册新连接
            self.clients[websocket] = {
                "user_id": user_id,
                "last_active": datetime.now()
            }
            self.user_connections[user_id].add(websocket)
            print(f"[{datetime.now()}] 用户 {user_id} 已连接")

            # 新用户连接时广播
            await self.broadcast("join", {"user_id": user_id})
            await self.broadcast("online_users", list(self.user_connections.keys()))

            # 发送在线用户列表
            await websocket.send(json.dumps({
                "type": "online_users",
                "users": list(self.user_connections.keys())
            }))
            

            # 接收消息循环
            async for message in websocket:
                if isinstance(message, bytes):
                    print("[收到二进制数据] 忽略")
                    return
                await self.handle_client_message(websocket, message)  # 处理消息
                    
        except websockets.exceptions.ConnectionClosedOK:
            print(f"[{datetime.now()}] 用户 {user_id} 正常断开")
        except Exception as e:
            print(f"[连接异常] {str(e)}")
            print(f"[{datetime.now()}] 用户 {user_id} 异常断开")
        finally:
            self._clean_connection(websocket)
            if user_id:
                await self.broadcast("leave", {"user_id": user_id})
                await self.handle_leave_room(user_id, websocket,0)
                
#                await self.broadcast("online_users", list(self.user_connections.keys()))


    async def handle_client_message(self, websocket, message):
        """ 处理来自客户端的消息 """
        try:
        	  
            payload = json.loads(message)
#            print(f"[收到消息]",payload)
            if payload.get("type") == "invite":
                await self.handle_invite(payload, websocket)
            elif payload.get("type") == "accept":
                await self.handle_accept(payload["user_id"], websocket)
            elif payload.get("type") == "get_online_users":
                # 发送在线用户列表
                await websocket.send(json.dumps({
                    "type": "online_users",
                    "users": list(self.user_connections.keys())
                }))
            elif payload.get("type") == "message":
                await self.broadcast("new_message", {
                    "user_id": self.clients[websocket]["user_id"],
                    "content": payload["content"]
                })
            elif payload.get("type") == "check_if_in_room":
                user_id = self.clients[websocket]["user_id"]
                room_id = self.get_room_id_by_user_id(user_id)
                if room_id is not None and user_id in self.rooms[room_id]:
                    await websocket.send(json.dumps({
                        "type": "hanging_room",
                        "room_id": room_id,
                        "users": list(self.rooms[room_id])
                    }))
                
            elif payload.get("type") == "leave_room":
                if "user_id" in payload:
                    await self.handle_leave_room(payload["user_id"], websocket,1);
                    
            elif payload.get("type") == "room_game_start":
                if "room_id" in payload:
                    await self.handle_room_game_start(payload["room_id"],websocket)

            elif payload.get("type") == "room_message":
                await self.handle_send_message_in_room(websocket, payload)

                
        except json.JSONDecodeError:
            print(f"[消息解析错误] {message}")
        except Exception as e:
            print(f"[消息处理错误] {str(e)}")

    async def handle_invite(self, payload, websocket):
        """ 处理用户的邀请请求 """
        inviter_id = self.clients[websocket]["user_id"]
        invited_user_id = payload["user_id"]

        if invited_user_id in self.user_connections:
            # 检查邀请的用户是否在线
            await self.send_invite(inviter_id, invited_user_id)
            print(f"[发送邀请]来自ID:",inviter_id," 至ID:",invited_user_id)
        else:
            await self.send_message(websocket, "user not online")

    async def send_invite(self, inviter_id, invited_user_id):
        """ 发送邀请给目标用户 """
        for ws in self.user_connections[invited_user_id]:
            await ws.send(json.dumps({
                "type": "invite",
                "inviter_id": inviter_id
            }))
        self.invites[invited_user_id] = (inviter_id, None)  # 注册邀请

    async def handle_accept(self, invited_user_id, websocket):
        """ 接受邀请 """
        if invited_user_id not in self.invites:
            await self.send_message(websocket, "no invite could be accepted")
            return

        inviter_id, room_id = self.invites[invited_user_id]
        room_id = f"{inviter_id}"  # 以邀请者 ID 创建房间

        if len(self.rooms[room_id]) < 2:  # 检查房间是否已满
            self.rooms[room_id].update([inviter_id, invited_user_id])
            for ws in self.user_connections[inviter_id]:
                await ws.send(json.dumps({"type": "room_joined", "room_id": room_id, "users": list(self.rooms[room_id])}))
            for ws in self.user_connections[invited_user_id]:
                await ws.send(json.dumps({"type": "room_joined", "room_id": room_id, "users": list(self.rooms[room_id])}))
            del self.invites[invited_user_id]  # 清除邀请
        else:
            await self.send_message(websocket, "the room is occupied")

        if room_id in self.pending_deletion:
            self.pending_deletion[room_id][0].cancel()
            del self.pending_deletion[room_id]
            print(f"[{datetime.now()}] 用户重新加入，取消房间 {room_id} 的删除")

    def get_room_id_by_user_id(self, user_id):
        """根据 user_id 获取 room_id"""
        for room_id, users in self.rooms.items():
            if user_id in users:
                return room_id
        return None  # 如果找不到 user_id 对应的 room_id

        
#    async def handle_leave_room(self, user_id, websocket):
#        """处理用户离开房间"""
#        room_id = self.get_room_id_by_user_id(user_id)
#        print(f"[{datetime.now()}]找到 Room ID: {room_id}")
#
#        if room_id is not None and user_id in self.rooms[room_id]:
#            del self.rooms[room_id]  # 删除房间
#
#            # 通知房间其他成员
#            await self.broadcast("room_delete", {
#                "room_id": room_id
#            })
#
#            print(f"[{datetime.now()}] 房间 {room_id} 已被删除")
    # 修改 handle_leave_room 方法
    async def handle_leave_room(self, user_id, websocket,isPlayerOffline):    
        """处理用户离开房间（带延迟删除）"""
        room_id = self.get_room_id_by_user_id(user_id)
        print(f"[{datetime.now()}] 检测到用户 {user_id} 离开房间 {room_id}")

        if isPlayerOffline == 0:
            if room_id is not None and user_id in self.rooms[room_id]:
                # 如果已有预定删除任务，先取消
                if room_id in self.pending_deletion:
                    self.pending_deletion[room_id][0].cancel()
                    print(f"[{datetime.now()}] 取消房间 {room_id} 的待定删除")

                # 安排新的延迟删除任务
                deletion_task = asyncio.create_task(
                    self.delayed_room_deletion(room_id)
                )
                self.pending_deletion[room_id] = (deletion_task, datetime.now())
                print(f"[{datetime.now()}] 房间 {room_id} 将在5秒后删除")
        else:
            if room_id is not None and user_id in self.rooms[room_id]:
                del self.rooms[room_id]  # 删除房间

                # 通知房间其他成员
                await self.broadcast("room_delete", {
                    "room_id": room_id
                })
    
                print(f"[{datetime.now()}] 房间 {room_id} 已被删除")
            

    # 添加新的延迟删除方法
    async def delayed_room_deletion(self, room_id):
        try:
            await asyncio.sleep(5)  # 等待5秒
        
            # 再次确认房间是否仍然需要删除
            del self.rooms[room_id]
            await self.broadcast("room_delete", {"room_id": room_id})
            print(f"[{datetime.now()}] 延迟删除房间 {room_id}")
        
            # 清理待删除记录
            if room_id in self.pending_deletion:
                del self.pending_deletion[room_id]
            
        except asyncio.CancelledError:
            print(f"[{datetime.now()}] 房间 {room_id} 删除任务已取消")
        except Exception as e:
            print(f"[延迟删除错误] {str(e)}")

    async def handle_room_game_start(self, room_id, websocket):
        """处理房间游戏开始"""
        if room_id not in self.rooms:
            print(f"[错误] 房间 {room_id} 不存在")
            return

        # 生成随机球 ID 数组（包含 1-7 和 9-15）
        ball_ids = list(range(1, 8)) + list(range(9, 16))
        random.shuffle(ball_ids)  # 打乱球的顺序

        # 获取房间内所有用户
        user_ids = list(self.rooms[room_id])  # 转换为列表
        if not user_ids:
            print(f"[错误] 房间 {room_id} 没有用户，无法开始游戏")
            return  # 提前返回，避免后续代码执行

        # 随机获取当前玩家
        current_player_id = random.choice(user_ids)
    
        # 遍历房间内用户，获取他们的 WebSocket 连接并发送消息
        for user_id in user_ids:
            if user_id in self.user_connections:
                for ws in self.user_connections[user_id]:  # 可能一个用户多个连接
                    try:
                        await ws.send(json.dumps({
                            "type": "game_start",
                            "room_id": room_id,
                            "randomBallIds": ball_ids,
                            "currentPlayerId": current_player_id
                        }))
                    except Exception as e:
                        print(f"[发送游戏开始消息错误] 用户 {user_id}: {e}")

        print(f"[{datetime.now()}] 房间 {room_id} 游戏已开始, 随机球 ID: {ball_ids}, 开球用户ID: {current_player_id}")

    async def handle_send_message_in_room(self, websocket, payload):
        """处理房间内消息发送"""
        sender_id = self.clients[websocket]["user_id"]
        room_id = self.get_room_id_by_user_id(sender_id)

        if not room_id:
            print(f"[错误] 用户 {sender_id} 不在任何房间内，无法发送消息")
            return
    
#        message_content = payload.get("message", "")
#        if not message_content:
#            print(f"[警告] 用户 {sender_id} 发送了空消息")
#            return
        

        # 发送消息给房间内的所有用户（除了自己）
        for user_id in self.rooms[room_id]:
            if user_id != sender_id and user_id in self.user_connections:
                for ws in self.user_connections[user_id]:  # 可能有多个 WebSocket 连接
                    try:
#                        await ws.send(json.dumps({
#                            "type": "room_message",
#                            "room_id": room_id,
#                            "sender": sender_id,
#                            "message": message_content
#                        }))
                        await ws.send(json.dumps(payload)); 
#                        print(f"[{datetime.now()}][房间消息]来自ID:",sender_id," 至ID:",user_id," ,发送消息")
                    except Exception as e:
                        print(f"[消息发送失败] 发送给 {user_id} 失败: {e}")

#        print(f"[{datetime.now()}] {sender_id} 在房间 {room_id} 发送消息: {payload}")



    async def send_message(self, websocket, message):
        """ 发送消息给指定的连接 """
        await websocket.send(json.dumps({"type": "message", "content": message}))

    async def start_server(self, host='0.0.0.0', port=25689):
        while True:
            try:
                async with websockets.serve(
                    self.connection_handler,
                    host,
                    port,
                    ping_interval=5,
                    ping_timeout=30,
                    max_size=1024 * 1024
                ):
                    print(f"服务器运行于 ws://{host}:{port}")
                    await asyncio.Future()
            except Exception as e:
                print(f"服务器发生错误: {e}, 5 秒后重启...")
                await asyncio.sleep(5)

if __name__ == "__main__":
    server = WebSocketServer()
    asyncio.run(server.start_server())
    