import tkinter as tk
from tkintermapview import TkinterMapView
from tkinter import ttk, messagebox
import socket
import threading

# Define UDP host and port
UDP_HOST = "0.0.0.0"  # Listen on all available interfaces
UDP_PORT = 12345  # The same UDP port used by the ESP32

# To hold the current thread
current_thread = None

class ThreadWithMap:
    def __init__(self, container, thread_text, severity, lat, lon):
        self.container = container
        self.thread_text = thread_text
        self.severity = severity
        self.lat = lat
        self.lon = lon
        self.validations = 0
        self.create_thread_with_map()

    def get_color(self):
        # Red, Orange, and Indigo color scheme for severity levels
        colors = {"Severe": "red", "Moderate": "orange", "Normal": "indigo", "Unknown": "grey"}
        return colors.get(self.severity, "grey")

    def create_thread_with_map(self):
        thread_frame = tk.Frame(self.container, bd=2, relief="solid", padx=20, pady=20, width=850)
        thread_frame.pack(fill="x", padx=30, pady=15)

        map_frame = tk.Frame(thread_frame, width=300, height=300)
        map_frame.pack(side="left", padx=10, pady=10)

        gmap_widget = TkinterMapView(map_frame, width=300, height=300)
        gmap_widget.pack(fill="both", expand=True)

        # Set position using received latitude and longitude
        gmap_widget.set_position(self.lat, self.lon, marker=True)
        gmap_widget.set_zoom(12)

        info_frame = tk.Frame(thread_frame)
        info_frame.pack(side="left", fill="both", expand=True, padx=20)

        text_label = tk.Label(info_frame, text=self.thread_text, anchor="w", justify="left", wraplength=400, font=("Arial", 16))
        text_label.pack()

        severity_label = tk.Label(info_frame, text=self.severity, bg=self.get_color(), fg="white", font=("Arial", 14, "bold"), padx=20, pady=5, width=20)
        severity_label.pack(pady=10)

        self.validation_label = tk.Label(info_frame, text=f"Validations: {self.validations}", font=("Arial", 12))
        self.validation_label.pack(pady=5)

        description_label = tk.Label(info_frame, text="Describe the pothole:", font=("Arial", 12))
        description_label.pack(pady=5)

        self.description_entry = tk.Text(info_frame, height=3, width=40)
        self.description_entry.pack(pady=5)

        submit_button = tk.Button(info_frame, text="Submit", command=self.submit_description, font=("Arial", 14, "bold"), padx=20, width=15)
        submit_button.pack(pady=5)

    def submit_description(self):
        description = self.description_entry.get("1.0", "end").strip()
        if description:
            self.validations += 1
            self.validation_label.config(text=f"Validations: {self.validations}")
            self.show_popup()
            self.description_entry.delete("1.0", "end")
        else:
            messagebox.showwarning("Submission", "Please enter a description before submitting.")

    def show_popup(self):
        popup = tk.Toplevel(self.container)
        popup.geometry("300x200")
        popup.title("Points Earned")
        
        label = tk.Label(popup, text="+5 pts", font=("Arial", 18, "bold"), pady=20)
        label.pack(expand=True)

        self.container.after(2500, lambda: popup.destroy())

# UDP Listener function
def listen_for_udp():
    """Listen for UDP packets and update the UI"""
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_HOST, UDP_PORT))  # Bind to UDP port
    print(f"Listening for UDP packets on port {UDP_PORT}...")

    while True:
        data, addr = sock.recvfrom(1024)  # Receive data (up to 1024 bytes)
        if data:
            try:
                message = data.decode("utf-8")
            except UnicodeDecodeError:
                print("Received data is not valid UTF-8")
                continue  # Skip this packet if it can't be decoded

            print(message)

            # Split the message into parts
            parts = message.split(",")

            if len(parts) == 3:  # If we have latitude, longitude, and severity
                lat, lon, severity = parts
            elif len(parts) == 2:  # If we only have latitude and longitude
                lat, lon = parts
                severity = "Unknown"  # Default severity if missing
            else:
                continue  # Skip invalid messages

            lat = float(lat)
            lon = float(lon)
            severity = severity.strip()

            # Update the UI with the received message
            update_ui(f"Pothole detected at Latitude: {lat}, Longitude: {lon}", severity, lat, lon)

# Function to update the UI with the received data
def update_ui(text, severity, lat, lon):
    """Update UI label with new data"""
    global current_thread
    # Create a new thread with map using the updated data
    current_thread = ThreadWithMap(content_frame, text, severity, lat, lon)

    # Ensure the canvas updates its scroll region after adding new thread content
    canvas.update_idletasks()
    canvas.configure(scrollregion=canvas.bbox("all"))

# Create Tkinter UI
root = tk.Tk()
root.title("Pothole Detection Interface with Maps")
root.geometry("900x500")
root.resizable(False, False)

# Create a Canvas widget with a Scrollbar
canvas = tk.Canvas(root, width=700)
canvas.pack(side="left", fill="both", expand=True)

scrollbar = ttk.Scrollbar(root, orient="vertical", command=canvas.yview)
scrollbar.pack(side="right", fill="y", padx=5)

canvas.configure(yscrollcommand=scrollbar.set)
canvas.bind("<Configure>", lambda e: canvas.configure(scrollregion=canvas.bbox("all")))

# Create the content frame that will hold dynamic content (pothole threads)
content_frame = tk.Frame(canvas)
canvas.create_window((0, 0), window=content_frame, anchor="nw")

# Start UDP listener in a separate thread
threading.Thread(target=listen_for_udp, daemon=True).start()

root.mainloop()
