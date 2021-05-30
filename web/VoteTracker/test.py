from colorama import Fore, Style, init
from pprint import pprint
from traceback import print_exc

import json, argparse, sys, random, string, time

import uuid
import requests

# Parse cmdline args
parser = argparse.ArgumentParser()
parser.add_argument("--UpVote", nargs=1, metavar=('NAME'), help="UpVote", dest="UpVote")
parser.add_argument("--DownVote", nargs=1, metavar=('NAME'), help="DownVote", dest="DownVote")
parser.add_argument("--GetScores", help="Get leaderboard scores only", action='store_true', dest="GetScores")

args = parser.parse_args()

    
init() #init colorama

#Initialise JSON-RPC endpoint


class Proxy():
    DEFAULT_HEADERS = {}

    def __init__(self, endpoint = 'http://127.0.0.1:5000/api', headers=None):
        self.service_url = endpoint
        self.headers = headers or self.DEFAULT_HEADERS

    def post(self, method, params = {}):
        """Performs the actual sending action and returns the result"""
        url_request = requests.post(self.service_url + "/" + method, params)
        print(url_request.request.body)
        print(url_request.status_code)
        return str(url_request.content.decode())

    def get(self, method):
        """Performs the actual sending action and returns the result"""
        url_request = requests.get(self.service_url + "/" + method)
        print(url_request.request.body)
        print(url_request.status_code)
        return str(url_request.content.decode())

# server = Proxy('http://morganrobertson.net/LTLeaderBoard/api')
server = Proxy(endpoint = 'http://127.0.0.1:5000/api')

failed_tests = False

def random_generator(size = 6, chars=string.ascii_letters):
    return ''.join(random.choice(chars) for x in range(size))

def test_check(): # Establish basic call to API endpoint and time the connection.
    print("Performing 'check' API call test.  Ensure's basic API connectivity:")
    try:
        startTime = time.time()
        response = server.get("check")
        endTime = time.time()
        print('check call took {:.3f} ms'.format((endTime-startTime)*1000.0))
        assert "Welcome" in response
        assert "error" not in response
        print(f'{Fore.GREEN}check test passed{Style.RESET_ALL}')
    except:
        print(f'{Fore.RED}check test failed. Fundamental problem with API server exists. Exiting...{Style.RESET_ALL}')
        global failed_tests
        failed_tests = True
        print_exc()
    finally:
        if failed_tests == True:
            sys.exit()
        print('Response:', response, '\n')

def test_UpVote(name = 'Morgan'):
    print("Performing 'UpVote' API call test:")
    response = server.post("UpVote", { "seed": name})
    try:
        assert "Score added" or "Score updated" in response
        assert "error" not in response
        print(f'{Fore.GREEN}UpVote test passed{Style.RESET_ALL}')
    except:
        print(f'{Fore.RED}UpVote test failed{Style.RESET_ALL}')
        global failed_tests
        failed_tests = True
        print_exc()
    finally:
        print('Response:', response, '\n')

def test_DownVote(name = 'Chicken'):
    print("Performing 'DownVote' API call test:")
    response = server.post("DownVote", { "seed": name})
    try:
        assert "Score added" or "Score updated" in response
        assert "error" not in response
        print(f'{Fore.GREEN}DownVote test passed{Style.RESET_ALL}')
    except:
        print(f'{Fore.RED}DownVote test failed{Style.RESET_ALL}')
        global failed_tests
        failed_tests = True
        print_exc()
    finally:
        print('Response:', response, '\n')

def test_UpVoteInvalidData(name = "`;Yr"):
    print("Performing 'UpVote' API call test with invalid data.  Server should error:")
    response = server.post("UpVote", { "seed": name})
    try:
        assert "Score added" not in response
        assert "error" in response
        print(f'{Fore.GREEN}Attempt UpVote with invalid data test passed{Style.RESET_ALL}')
    except:
        print(f'{Fore.RED}Attempt UpVote with invalid data test failed{Style.RESET_ALL}')
        global failed_tests
        failed_tests = True
        print_exc()
    finally:
        print('Response:', response, '\n')


def test_GetScores(name = None): # GetScores and measure the time it takes
    print("Performing 'Getscores' API call test:")
    startTime = time.time()
    response = server.get("GetScores")
    endTime = time.time()
    print('GetScores call took {:.3f} ms'.format((endTime-startTime)*1000.0))
    try:
        if name != None:
            assert "name" in str(response)     
        if not args.GetScores:   # Only perform the following check if we are performing all tests.
            assert 'going' in str(response)
            assert '10' in str(response)
        assert "error" not in str(response)
        # score_data = json.loads(response)
        # pprint(score_data)
        print(f'{Fore.GREEN}GetScores test passed{Style.RESET_ALL}')
    except:
        print(f'{Fore.RED}GetScores test failed{Style.RESET_ALL}')
        global failed_tests
        failed_tests = True
        print_exc()
    finally:
        print('Raw Response:', response, '\n')
    

if __name__ == "__main__":
    if args.UpVote:
        test_UpVote(args.UpVote[0])
    elif args.DownVote:
        test_UpVote(args.DownVote[0])
    elif args.GetScores:
        test_GetScores()
    else:
        # Try a seed
        random_name = random_generator()
        test_check()
        test_UpVote(name = random_name)
        test_UpVoteInvalidData()
        test_UpVote(name = random_name)
        test_DownVote(name = random_name)
        test_GetScores()

        # Try another seed
        random_name = random_generator() + " 123"
        test_UpVote(name = random_name)
        test_UpVoteInvalidData()
        
        for x in range(5):
            test_UpVote(name = random_name)
        for x in range(2):
            test_DownVote(name = random_name)

        test_GetScores()

        if failed_tests == False:
            print(f'{Fore.GREEN}ALL TESTS PASSED!{Style.RESET_ALL}')
        else: 
            print(f'{Fore.RED}OH NO! THERE BE ERRORS & TEST FAILURES!{Style.RESET_ALL}')
