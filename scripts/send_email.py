import sys
import os
import json
import urllib.request

if len(sys.argv) < 2:
    sys.exit(1)

tmp_file = sys.argv[1]

try:
    with open(tmp_file, 'r') as f:
        lines = f.readlines()
        
    if len(lines) < 3:
        sys.exit(1)
        
    address = lines[0].strip()
    subject = lines[1].strip()
    body = "".join(lines[2:])
    
    # Path to .env file relative to this script's parent directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    # Assuming .env is in haven-support/
    parent_dir = os.path.dirname(script_dir)
    env_file = os.path.join(parent_dir, '.env')
    
    resend_api_key = ''
    resend_from_email = 'noreply@havenmud.net' # fallback
    
    if os.path.exists(env_file):
        with open(env_file, 'r') as e:
            for line in e:
                line = line.strip()
                if line.startswith('RESEND_API_KEY='):
                    resend_api_key = line.split('=', 1)[1].strip().strip('"\'')
                elif line.startswith('RESEND_FROM_EMAIL='):
                    resend_from_email = line.split('=', 1)[1].strip().strip('"\'')
                    
    if not resend_api_key:
        # Check environment variable as fallback
        resend_api_key = os.environ.get('RESEND_API_KEY', '')
        if not resend_api_key:
            sys.exit(1)
            
    # Also check env for from email
    env_from = os.environ.get('RESEND_FROM_EMAIL')
    if env_from:
        resend_from_email = env_from

    url = 'https://api.resend.com/emails'
    headers = {
        'Authorization': f'Bearer {resend_api_key}',
        'Content-Type': 'application/json',
        'User-Agent': 'HavenMUD-Email-Client/1.0'
    }
    
    payload = {
        "from": resend_from_email,
        "to": [address],
        "subject": subject,
        "text": body
    }
    
    data = json.dumps(payload).encode('utf-8')
    req = urllib.request.Request(url, data=data, headers=headers, method='POST')
    
    with urllib.request.urlopen(req) as response:
        pass
        
except urllib.error.HTTPError as e:
    print(f"HTTP Error: {e.code}")
    print(e.read().decode('utf-8'), file=sys.stderr)
except Exception as e:
    import traceback
    traceback.print_exc(file=sys.stderr)
finally:
    if os.path.exists(tmp_file):
        os.remove(tmp_file)
