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
 * FILE:        MyClient.java
 *
 * DESCRIPTION: This file contains the client for homework 1.
 *
 */

import java.io.*;
import java.net.Socket;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

public class MyClient {

    private final static String DEFAULT_FILE = "client_file.txt";
    private final static String LOCALHOST = "127.0.0.1";

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
        File log = new File("client_log.txt");
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
                        System.out.println("Connecting to server...");
                        log_bw.write("Connecting to server...\n");
                        Socket socket = new Socket(LOCALHOST, port_number);
                        System.out.println("Connected to server at port " + port_number + ".");
                        log_bw.write("Connected to server at port " + port_number + ".\n");
                        System.out.println("Receiving file...");
                        log_bw.write("Receiving file...\n");
                        File client_file = new File(DEFAULT_FILE);
                        try {
                            client_file.createNewFile();
                        } catch (IOException e) {
                            System.out.println("Error: unable to create new file.");
                            log_bw.write("Error: unable to create new file.\n");
                        }
                        /* https://docs.oracle.com/javase/tutorial/networking/sockets/readingWriting.html */
                        BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                        BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
                        FileWriter fw = new FileWriter(client_file, true);
                        BufferedWriter bw = new BufferedWriter(fw);
                        String input;
                        while((input = br.readLine()) != null) {
                            bw.write(input + "\n");
                        }

                        System.out.println("File received.");
                        log_bw.write("File received.\n");

                        // Clean up
                        socket = null;
                        bw.close();
                        System.out.println("Connection closed.");
                        log_bw.write("Connection closed.\n");
                    } catch (IOException e) {
                        System.out.println("Server error: " + e);
                        log_bw.write("Server error: " + e + "\n");
                    }
                }
            }
            log_bw.write("\n");
            log_bw.close();
        } catch(IOException e) {
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
