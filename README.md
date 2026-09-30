# 🚗 Parking Lot Management System

A **console-based Parking Lot Management System written in C** that provides an organized way to manage vehicle parking, check-ins, check-outs, parking slots, revenue, logs, and customer feedback.

The system supports separate **User** and **Supervisor** interfaces and uses text files to maintain data even after the program is closed.

---

## 📌 Project Overview

The Parking Lot Management System is designed to simulate the basic operations of a real-world parking facility.

Users can:

* Check vehicles into the parking lot
* Receive a parking slot and ticket number
* Check vehicles out using their ticket number
* Automatically calculate parking fees
* View currently occupied parking slots
* Submit ratings and feedback

Supervisors can:

* View total revenue
* View currently parked vehicles
* Access check-in and check-out logs
* View customer feedback
* Reset revenue
* Clear system logs
* Reset the entire parking system

The system supports a maximum of **20 parking slots**.

---

## ✨ Features

### 👤 User Features

#### 🚘 Vehicle Check-In

Users can check in:

* Car
* Bike
* Van

The system automatically:

1. Finds the first available parking slot
2. Records the vehicle type
3. Records the number plate
4. Stores the check-in time
5. Assigns a ticket number
6. Saves the parking information
7. Creates a check-in log

---

### 🎫 Ticket System

Each parking slot generates a ticket number using a predefined ticket offset.

```text
Ticket Number = Slot Number + 17900
```

For example:

```text
Slot 1 → Ticket 17901
Slot 2 → Ticket 17902
Slot 3 → Ticket 17903
```

---

### 🚗 Vehicle Check-Out

Vehicles can be checked out using their ticket number.

The system:

* Identifies the parked vehicle
* Calculates the parking duration
* Calculates the parking fee
* Updates total revenue
* Generates a checkout log
* Displays a receipt
* Frees the parking slot

---

### 💰 Automatic Fee Calculation

Parking fees are calculated according to vehicle type and parking duration.

| Vehicle  |         Rate |
| -------- | -----------: |
| 🚗 Car   | Rs. 100/hour |
| 🏍️ Bike |  Rs. 50/hour |
| 🚐 Van   | Rs. 150/hour |

The system rounds partial hours upward when calculating the final parking duration.

---

### 🅿️ Parking Slot Management

The system maintains **20 parking slots**.

It automatically searches for the first available slot during check-in.

If every slot is occupied, the system displays:

```text
Parking is FULL!
```

---

### 📊 Parking Status

Users can view the current parking status and see how many slots are occupied.

Example:

```text
Total Occupied: 7 / 20
```

If there are no parked vehicles:

```text
All slots are available.
```

---

### ⭐ Feedback & Rating

Users can submit:

* Rating from 1–5
* Written comments

Feedback is stored in a dedicated text file along with the submission time.

---

## 🔐 Supervisor Panel

The system contains a password-protected supervisor menu.

The supervisor can:

```text
1. View Total Revenue
2. View Parked Vehicles
3. Open Check-in Log
4. Open Check-out Log
5. Open Feedback Log
6. Reset Revenue
7. Reset Logs
8. Reset Entire System
9. Back
```

> **Note:** The supervisor password is defined directly in the source code. For a real-world application, credentials should not be hard-coded.

---

## 🗂️ Data Persistence

The project uses text files to store information.

| File               | Purpose                                    |
| ------------------ | ------------------------------------------ |
| `parking_data.txt` | Stores current parking/vehicle information |
| `revenue.txt`      | Stores total revenue                       |
| `checkin_log.txt`  | Stores vehicle check-in records            |
| `checkout_log.txt` | Stores vehicle check-out records           |
| `feedback.txt`     | Stores user ratings and comments           |

## These files are automatically created if they do not already exist.

## 🛠️ Technologies Used

* **C Programming**
* Standard C Libraries

  * `stdio.h`
  * `string.h`
  * `time.h`
  * `ctype.h`
  * `stdlib.h`
* File Handling
* Structures
* Arrays
* Functions
* Conditional Statements
* Loops
* Time and Date Handling
* Console-Based User Interface

---

## 🧠 Concepts Demonstrated

This project demonstrates several fundamental C programming concepts:

### Structures

Vehicle information is represented using a `struct Vehicle` containing:

* Vehicle type
* Number plate
* Occupancy status
* Slot number
* Check-in time
* Parking fee

### Arrays

The parking lot is represented using an array of 20 `Vehicle` structures.

```c
struct Vehicle vehicles[MAX_SLOTS];
```

### File Handling

The program uses file operations to save and retrieve persistent data.

Examples include:

```c
fopen()
fscanf()
fprintf()
fgets()
fclose()
```

### Time Handling

The project uses C's time functionality to record check-in and check-out times and calculate parking duration.

### Input Validation

The program validates:

* Menu choices
* Vehicle types
* Ticket numbers
* Ratings
* Invalid input

---

## 📁 Project Structure

```text
Parking-Lot-Management-System/
│
├── ParkingLotManagementSystem.c
│
├── parking_data.txt
├── revenue.txt
├── checkin_log.txt
├── checkout_log.txt
├── feedback.txt
│
└── README.md
```

> The `.txt` files are generated/used by the program for persistent storage and logs.

---

## ▶️ How to Run

### 1. Clone the Repository

```bash
git clone https://github.com/YOUR-USERNAME/Parking-Lot-Management-System.git
```

### 2. Navigate to the Project

```bash
cd Parking-Lot-Management-System
```

### 3. Compile the Program

Using GCC:

```bash
gcc ParkingLotManagementSystem.c -o ParkingLotManagementSystem
```

### 4. Run

On Windows:

```bash
ParkingLotManagementSystem.exe
```

On Linux/macOS:

```bash
./ParkingLotManagementSystem
```

---

## 🖥️ Main Menu

When the program starts, users are presented with:

```text
=========== PARKING LOT MANAGEMENT SYSTEM ===========

1. User
2. Supervisor
3. Exit
```

The User menu provides parking-related operations, while the Supervisor menu provides administrative functionality.

---

## 🔄 System Workflow

```text
             ┌───────────────────┐
             │      Program      │
             │       Starts      │
             └─────────┬─────────┘
                       │
                       ▼
             ┌───────────────────┐
             │    Main Menu      │
             └─────────┬─────────┘
                       │
              ┌────────┴────────┐
              ▼                 ▼
        ┌───────────┐     ┌─────────────┐
        │   User    │     │ Supervisor  │
        └─────┬─────┘     └──────┬──────┘
              │                  │
       ┌──────┼───────┐    ┌─────┼──────────┐
       ▼      ▼       ▼    ▼     ▼          ▼
    Check-in Check-out Slots Revenue       Logs
       │      │       │    │     │          │
       └──────┴───────┴────┴─────┴──────────┘
                       │
                       ▼
              Persistent Storage
                       │
                       ▼
                 Text Files
```

---

## 📈 Future Improvements

Possible improvements for future versions include:

* Graphical User Interface (GUI)
* Database integration using MySQL/SQLite
* Multiple parking zones
* Different pricing plans
* Online payment support
* QR-code based tickets
* User accounts
* Supervisor username/password authentication
* Search functionality
* Parking history
* Vehicle owner information
* Better reporting and analytics
* Cross-platform log viewing

---

## ⚠️ Current Limitations

This version is a console-based academic project and has some limitations:

* Data is stored in plain text files.
* Supervisor authentication uses a password stored in the source code.
* The system is designed around a fixed maximum of 20 slots.
* The log-opening functionality uses Windows Notepad commands.
* It does not use a database.
* It does not provide a graphical interface.

---

## 🎓 Project Purpose

This project was developed as an academic project to demonstrate practical implementation of fundamental **C programming concepts**, including:

* Structures
* Arrays
* Functions
* File handling
* String manipulation
* Time handling
* Input validation
* Menu-driven programming

It demonstrates how these concepts can be combined to build a functional real-world-inspired management system.

---

## 👨‍💻 Author

**Your Name**

Computer Science Student

GitHub: `https://github.com/YOUR-USERNAME`

---

## 📄 License

This project is intended primarily for **educational and academic purposes**.

You are welcome to study and modify the code for learning purposes.

---

⭐ If you found this project useful, consider giving the repository a **star**!

