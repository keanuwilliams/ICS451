/*
 *
 * NAME:        Keanu Williams
 *
 * HOMEWORK:    1
 *
 * CLASS:       ICS 451
 *
 * INSTRUCTOR:  Ravi Narayan
 *
 * DATE:        September 14, 2019
 *
 * FILE:        MyServer.java
 *
 * DESCRIPTION: This file contains the server for homework 1.
 *
 */

import java.io.*;
import java.net.Socket;
import java.net.ServerSocket;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

public class MyServer {

    private final static String DEFAULT_FILE = "server_file.txt";

    /*
     *
     * FUNCTION:    main
     *
     * DESCRIPTION: The main function of the program.
     *
     * PARAMETERS:  String[] args - the command-line arguments.
     *
     */

    public static void main(String[] args) {

        int port_number = -1;

        // For the logger
        File log = new File("server_log.txt");
        try {
            if(!log.exists()) {
                log.createNewFile();
            }
            FileWriter log_fw = new FileWriter(log, true);
            BufferedWriter log_bw = new BufferedWriter(log_fw);
            log_bw.write((new SimpleDateFormat("MM/dd/yy HH:mm:ss").format(new Date())).toString());
            log_bw.write("\n");

            // Begin error checking
            if(!(args.length == 1)) { // Check the amount of arguments
                System.out.println("Error: incorrect number of arguments.");
                log_bw.write("Error: incorrect number of arguments.\n");
                System.out.println("Please use the correct form in the command line:\n> java MyServer [port number]");
                log_bw.write("Please use the correct form in the command line:\n> java MyServer [port number]\n");
            }
            else if (is_integer(args[0]) == false) { // Check if port number inserted is an integer
                System.out.println("Error: port number is not an integer.");
                log_bw.write("Error: port number is not an integer.\n");
            }
            else {
                port_number = Integer.parseInt(args[0]);
                if(port_number < 0) { // Check if port was updated or if it was a negative.
                    System.out.println("Error: port number is negative.");
                    log_bw.write("Error: port number is negative.\n");
                }
                else { // If there was no issues w/ port number, continue.
                    try {
                        ServerSocket server_socket = new ServerSocket(port_number);
                        System.out.println("Server started.\nListening for connections on port " + port_number + "...");
                        log_bw.write("Server started.\nListening for connections on port " + port_number + "...\n");
                        Socket client_socket = server_socket.accept();
                        System.out.println("Connection made: " + client_socket);
                        log_bw.write("Connection made: " + client_socket + "\n");
                        System.out.println("Now sending file...");
                        log_bw.write("Now sending file...\n");
                        File server_file = new File(DEFAULT_FILE); // The file to send
                        FileInputStream fis = new FileInputStream(server_file); // Open a connection to the file
                        BufferedReader br = new BufferedReader(new InputStreamReader(fis)); // Used to read the file
                        // Used to write the file
                        PrintWriter out = new PrintWriter(client_socket.getOutputStream(), true);
                        boolean done = false;
                        while(!done) {
                            String line = br.readLine(); // Read each line
                            if(line != null) {
                                out.println(line); // Write the line read
                            }
                            else {
                                done = true;
                            }
                        }
                        System.out.println("File sent.");
                        log_bw.write("File sent.\n");

                        // Clean Up
                        client_socket = null;
                        fis.close();
                        server_socket.close();
                        System.out.println("Connection closed.");
                        log_bw.write("Connection closed.\n");
                    } catch(FileNotFoundException e) {
                        System.out.println("Error: 'server_file.txt' does not exist.");
                        log_bw.write("Error: 'server_file.txt' does not exist.\n");
                    } catch(IOException e) {
                        System.out.println("Server error: " + e);
                        log_bw.write("Server error: " + e + "\n");
                    }
                }
            }
            log_bw.write("\n");
            log_bw.close();
        } catch (IOException e) {
            System.out.println("Error: unable to log.");
        }
    }

    /*
     *
     * FUNCTION:    is_integer
     *
     * DESCRIPTION: Checks if the string is an integer.
     *
     * PARAMETERS:  String s - the string to check.
     *
     * RETURNS:     true  - if the string is an integer.
     *              false - if the string is not an integer.
     *
     */

    private static boolean is_integer(String s) {
        boolean check = false; // the flag to be returned
        try {
            Integer.parseInt(s);
            check = true;
        } catch (NumberFormatException e) {
            check = false;
        }
        return check;
    }

}
