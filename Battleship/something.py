import re

def parse_and_process_message(message):
    #Define the expected pattern using regular expression
    pattern = r'BSR:([A-Za-z]);([A-Ja-j]);([0-9]|10);([0-3])'

    # Try to match the message with the pattern
    match = re.match(pattern, message)

    if match:
        # Extract individual components from the matched groups
        first_char, second_char, third_param, fourth_param = match.groups()
        # Convert third_param to an integer, subtract 1
        third_param = int(third_param) - 1

        # Reconstruct the message with the modified third parameter
        return f"BSR:{first_char};{second_char};{third_param};{fourth_param}"
    
    pattern1 = r'PGS:([A-Za-z]);(10|[0-9])$'
    match1 = re.match(pattern1, message)
    if match1:
        first_char, second_char = match1.groups()
        print(str(second_char) + ":")
        second_char = int(second_char) - 1
        return f"PGS:{first_char};{second_char}"
    
    return message

# Get user input for the message
user_input = input("Enter the message: ")

# Call the function with the user input
print(parse_and_process_message(user_input))
