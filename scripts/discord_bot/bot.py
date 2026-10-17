import discord
import asyncio
import json
import os

# Configuration
MUD_HOST = '127.0.0.1'
MUD_PORT = 2001
DISCORD_TOKEN = os.environ.get('DISCORD_TOKEN', '')
OOC_CHANNEL_ID = 1550461496985911446 # Replace with actual Discord channel ID

intents = discord.Intents.default()
intents.message_content = True
client = discord.Client(intents=intents)

mud_reader = None
mud_writer = None

async def connect_to_mud():
    global mud_reader, mud_writer
    while True:
        try:
            print(f"Connecting to MUD at {MUD_HOST}:{MUD_PORT}...")
            mud_reader, mud_writer = await asyncio.open_connection(MUD_HOST, MUD_PORT)
            print("Connected to MUD bot port!")
            
            while True:
                data = await mud_reader.readline()
                if not data:
                    print("Connection to MUD closed.")
                    break
                    
                try:
                    message = json.loads(data.decode('utf-8'))
                    await handle_mud_message(message)
                except json.JSONDecodeError:
                    print(f"Received invalid JSON from MUD: {data}")
                    
        except ConnectionRefusedError:
            print("MUD connection refused. Retrying in 5 seconds...")
        except Exception as e:
            print(f"Error connecting to MUD: {e}")
            
        await asyncio.sleep(5)

async def handle_mud_message(message):
    event = message.get("event")
    
    if event == "who_list":
        players = message.get("players", [])
        immortals = message.get("immortals", [])
        
        response = ""
        if immortals:
            response += f"**Immortals online:**\n" + "\n".join(immortals) + "\n\n"
        if players:
            response += f"**Players online:**\n" + "\n".join(players) + "\n\n"
            
        if not players and not immortals:
            response = "No players currently online."
            
        response = response.strip()
            
        channel = client.get_channel(OOC_CHANNEL_ID)
        if channel:
            await channel.send(response)
            
    elif event == "chat":
        mud_channel = message.get("channel")
        sender = message.get("sender")
        msg = message.get("message")
        
        if mud_channel == "OOC":
            discord_channel = client.get_channel(OOC_CHANNEL_ID)
            if discord_channel:
                await discord_channel.send(f"**[{mud_channel}] {sender}:** {msg}")

@client.event
async def on_ready():
    print(f'Logged in as {client.user}')
    client.loop.create_task(connect_to_mud())

@client.event
async def on_message(message):
    if message.author == client.user:
        return

    if message.content.startswith('!who'):
        if mud_writer:
            req = {"action": "who"}
            mud_writer.write((json.dumps(req) + "\n").encode('utf-8'))
            await mud_writer.drain()
            
    elif message.channel.id == OOC_CHANNEL_ID:
        if message.content.startswith('!'):
            return # Ignore commands
            
        if mud_writer:
            # Relay to MUD
            req = {
                "action": "chat",
                "channel": "OOC",
                "sender": message.author.display_name,
                "message": message.content
            }
            mud_writer.write((json.dumps(req) + "\n").encode('utf-8'))
            await mud_writer.drain()

if __name__ == "__main__":
    client.run(DISCORD_TOKEN)
