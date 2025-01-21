# python_tools/ai_handler.py

import json
import sys

def main():
    # Read input from C++/C
    for line in sys.stdin:
        
        #Convert JSON encoded string to python object
        data = json.loads(line.strip()) #Strip leading/trailing whtiespace \n \t and spaces before loading
        result = {"response": f"Processed input: {data['message']}"}

         # Send the result back to C++/C
        print(json.dumps(result))
        sys.stdout.flush()

if __name__ == "__main__":
    main() 
