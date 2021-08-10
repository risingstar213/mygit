import requests
from urllib import request
from urllib import error
def http_request1(url, username, password, data= None):
    """Make an authenticated HTTP request to given URL (GET by default,
    POST if "data" is not None).
    """
    f = open('C:/Users/star/Desktop/test/data', 'rb')
    data = f.read()
    f.close()
    password_manager = request.HTTPPasswordMgrWithDefaultRealm()
    password_manager.add_password(None, url, username, password)
    auth_handler = request.HTTPBasicAuthHandler(password_manager)
    opener = request.build_opener(auth_handler)
    try:
        f = opener.open(url, data=data)
        ff = open('C:/Users/star/Desktop/test/test', 'w')
        byte = f.read()
        ff.write(byte.decode())
        ff.close()
        return 1
    except error.HTTPError as e:
        print(e.code, '\n', e.reason, '\n', e.headers)
        return 0
def http_request(url, username, password):
    response = requests.get(url, auth=(username, password))
    response.raise_for_status()
    #print(response.content)
    f = open('C:/Users/star/Desktop/test/test', 'w')
    f.write(response.content.decode())
    f.close()
    return 1