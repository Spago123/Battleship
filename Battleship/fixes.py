import serial
import threading
import tkinter as tk
import tkinter.font as font

SIZE = 10  # Assuming SIZE is known or provided
prefix = "M:"



def read_bit(arr, bit_position):
    return (arr[bit_position // 8] >> (bit_position % 8)) & 1

SIZE = 10

def READ_BIT(arr, bit_position):
    byte_position = bit_position // 8
    bit_offset = bit_position % 8

    byte = ord(arr[byte_position])  # Get the ASCII value of the character at the byte position
    return (byte >> (7 - bit_offset)) & 1



def decompress_board(str_board):
    board = [[0 for _ in range(SIZE)] for _ in range(SIZE)]

    for i in range(SIZE * SIZE):
        j = i * 2
        field = read_bit(str_board, j + 1)  # Read the value at position j + 1
        field <<= 1  # Shift the rightmost bit one position to the left
        field |= read_bit(str_board, j)  # Perform a bitwise OR with the previous bit
        print("Here: " + str(field))
        if field == 0:  # Assuming EMPTY is represented by 0
            board[i // SIZE][i % SIZE] = ' '
        elif field == 1:  # Assuming BOAT is represented by 1
            board[i // SIZE][i % SIZE] = 'o'
        elif field == 2:
            board[i // SIZE][i % SIZE] = 'x'
        elif field == 3:
            board[i // SIZE][i % SIZE] = '+'

    return board

def get_board_string(board):
    board_str = "_A_B_C_D_E_F_G_H_I_J_\n"

    for i in range(SIZE):
        for j in range(SIZE):
            board_str += "|"
            if board[i][j] == ' ':
                board_str += "_"
            else:
                board_str += board[i][j]
        board_str += f"|{i + 1}\n"

        print(board_str)
    return board_str



# Serial port configuration
ser = serial.Serial('COM7', 9600 * 4, timeout=None)  # Change 'COM8' to your serial port

def send_message(event=None):
    message = entry.get()
    txt = message
    entry.delete(0, tk.END)

    for i in range(30 - len(txt)):
        txt = txt + '\0'


    ser.write(txt.encode('latin-1'))
    received_messages.insert(tk.END, "\nPC: " + message)  # Display sent message on the screen

def receive_message():
    tab_size = 4  # Adjust as needed for your layout
    
    # Create a monospaced font
    monospace_font = font.Font(family='Courier', size=12)
    dataList = ["\0"] * 30
    cnt = 0
    
    while True:
        if ser.in_waiting:
            
            dataList[cnt] = ser.read(1).decode('latin-1').strip()
            cnt = cnt + 1
            print(''.join(dataList))
            
            if dataList[0] == prefix[0] and dataList[1] == prefix[1] and cnt == 30:
                #cnt = 0
                received_data = ''.join(dataList)
                #print(received_data + "\n")
                received_messages.insert(tk.END, "\nBoard: " + "\n")  # Display received message on the screen
                received_data = received_data[len(prefix):] 
                received_data = get_board_string(decompress_board(received_data.encode('latin-1')))
                #received_messages.insert(tk.END, "Your board:\n")
            
                lines = received_data.split("\n")
                for line in lines:
                    received_messages.insert(tk.END, line + "\n")
                    received_messages.tag_add("custom_style", "1.0", "end")
                    received_messages.tag_config("custom_style", foreground="black", background="white")  # Example styling, can be adjusted
                
                dataList.clear()
                dataList = ["\0"] * 30

            elif cnt == 30:
                cnt = 0
                #print(cnt)
                #received_data = ''.join(dataList)
                #print(received_data)
                received_messages.insert(tk.END, "\nBoard: " + ''.join(dataList))  # Display received message on the screen
                dataList.clear()
                dataList = ["\0"] * 30
                received_messages.tag_configure('monospace', font=monospace_font)
                received_messages.tag_add('monospace', 'end-1l', 'end')

# GUI setup
root = tk.Tk()
root.title("UART Chat")

# Enable resizing in both directions
root.resizable(1, 1)

frame = tk.Frame(root)
frame.pack(pady=10, expand=True, fill=tk.BOTH)  # Allow frame to expand in both directions
frame.pack_propagate(False)  # Prevent frame from automatically resizing

label = tk.Label(frame, text="Enter Message:", font=("Arial", 12))
label.pack(side=tk.LEFT)

entry = tk.Entry(frame, width=40, font=("Arial", 12))
entry.bind("<Return>", send_message)
entry.pack(side=tk.LEFT, padx=10)

send_button = tk.Button(frame, text="Send", command=send_message, font=("Arial", 12))
send_button.pack(side=tk.LEFT)

received_messages = tk.Listbox(root, width=70, height=15, font=("Arial", 12))
received_messages = tk.Text(root, font=('Courier', 10))  # 'Courier' is a monospaced font
received_messages.pack(expand=True, fill=tk.BOTH)  # Allow text widget to expand in both directions
received_messages.pack_propagate(False)  # Prevent text widget from automatically resizing

label_received = tk.Label(root, text="Received Messages:", font=("Arial", 12))
label_received.pack()

# Create a thread to handle receiving messages
receive_thread = threading.Thread(target=receive_message, daemon=True)
receive_thread.start()

root.mainloop()
