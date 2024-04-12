# AmangLyDriver

This project consists of a Windows kernel-mode driver and a user-mode application designed for interacting with the driver to manipulate memory in another process. It's primarily geared towards game enhancement, specifically targeting the game **"cs2.exe"** (Counter-Strike 2D).

## What's Inside?

### Kernel-mode Driver (`AmangLyDriver`)

This component enables user-mode applications to:
- **Attach** to a specified process
- **Read** its memory contents
- **Write** to it.

It provides a set of Input/Output Control (**IOCTL**) codes for tasks such as process attachment, memory reading, and memory writing. Additionally, it handles incoming requests from user-mode applications through I/O Request Packets (**IRPs**).

### User-mode Application (`client.exe`)

The user-mode application communicates with the kernel-mode driver to perform memory operations on a chosen target process. It leverages the Windows API to locate the process ID (PID) of the target process. Once connected to the driver, it can read and write memory in the target process. Specifically tailored for game hacking, it offers features like enabling infinite jumping by manipulating memory values.

## How to Use?

1. **Compile and Load the Driver**:
   ```
   Compile the kernel-mode driver (AmangLyDriver) and load it into the Windows kernel.
   ```

2. **Run the User-mode Application**:
   ```
   Launch the user-mode application (client.exe), which acts as the interface for interacting with the driver.
   ```

3. **Choose the Target Process**:
   ```
   Specify the name of the process you want to manipulate (e.g., "cs2.exe").
   ```

4. **Enjoy Enhanced Gameplay**:
   ```
   Utilize the provided features within the game to enhance your gaming experience. For example, you can achieve effects like infinite jumping.
   ```

## Important Notes

- **Use with Caution**:
  ```
  Modifying memory of other processes, especially in gaming environments, may violate terms of service and lead to penalties such as bans.
  ```

- **Educational Purposes Only**:
  ```
  This project is intended for educational purposes and should not be used for unethical or malicious activities.
  ```

---

Feel free to customize and expand upon this README.md summary as needed, providing additional details or instructions specific to your project.
