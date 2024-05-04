### AmangLyDriver Project

This project includes a Windows kernel-mode driver and a user-mode application, primarily designed for enhancing gameplay in the game "Counter-Strike2" cs2.exe by manipulating game memory.

#### Components:

- **Kernel-mode Driver (`AmangLyDriver`)**: Allows user applications to attach to another process to read and write its memory.

- **User-mode Application (`client.exe`)**: Interacts with the driver to modify memory in the game process, enabling features like infinite jumping.

#### Usage:

1. **Compile and Load the Driver**: Prepare the driver and load it into the Windows kernel.
2. **Run the User-mode Application**: Start `client.exe` to connect with the driver.
3. **Choose the Target Process**: Select "cs2.exe" as the process to manipulate.
4. **Enhance Your Gameplay**: Use the application to activate game enhancements.

#### Important Notes:

- **Use Responsibly**: Modifying game memory can breach game terms of service and lead to consequences like bans.
- **For Educational Use**: This project is meant for learning purposes and should not be used inappropriately.
