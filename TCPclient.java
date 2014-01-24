/*
 * TCPclient.java
 * Systems and Networks II
 * Project 1
 *
 * This file describes the functions to be implemented by the TCPclient class
 * You may also implement any auxiliary functions you deem necessary.
 */
import java.io.*;
import java.net.*;



public class TCPclient
{
	private Socket _socket; // the socket for communication with a server
	private static String sendString; //the string sent to the server
	private static String receivedString; //the string received from the server
	private BufferedReader stringIn;
	private BufferedWriter stringOut;
	private PrintWriter out;
	
	/**
	 * Constructs a TCPclient object.
	 */
	public TCPclient()
	{
		sendString = new String("<reply> hello world </reply>");
		receivedString = new String();
	}
	
	/**
	 * Creates a streaming socket and connects to a server.
	 *
	 * @param host - the ip or hostname of the server
	 * @param port - the port number of the server
	 *
	 * @return - 0 or a negative number describing an error code if the connection could not be established
	 */
	public int createSocket(String host, int port)
	{
		int error = 0;
		try {
			_socket = new Socket(host, port);
		} catch (IOException e) {
			error = -1;
			e.printStackTrace();
		}
		return error;
	}

	/**
	 * Sends a request for service to the server. Do not wait for a reply in this function. This will be
	 * an asynchronous call to the server.
	 * 
	 * @request - the request to be sent
	 *
	 * @return - 0, if no error; otherwise, a negative number indicating the error
	 */
	public int sendRequest(String request)
	{
		int error = 0;
		try {
			out = new PrintWriter(_socket.getOutputStream(), true);
			out.println(request);
		} catch (IOException e) {
			System.err.println("Error could not send request to server\n");
			e.printStackTrace();
		}
		return error;
	}
	
	/**
	 * Receives the server's response. Also displays the response in a
	 * clear and concise format.
	 *
	 * @return - the server's response or NULL if an error occured
	 */
	public String receiveResponse()
	{
		String response = new String(); 
		try {
			stringIn = new BufferedReader(new InputStreamReader(_socket.getInputStream()));
		} catch (IOException e) {
			System.err.println("Error: could not receive input from server\n");
			e.printStackTrace();
		}
		response = stringIn.toString();
		return response;
	}
	
	/*
    * Prints the response to the screen in a formatted way.
    *
    * response - the server's response as an XML formatted string
    *
    */
	public static void printResponse(String response)
	{
		System.out.println(response + "\n");
	}
 

	/*
	 * Closes an open socket.
	 *
    * @return - 0, if no error; otherwise, a negative number indicating the error
	 */
	public int closeSocket() 
	{
		int error = 0;
		try {
			_socket.close();
		} catch (IOException e) {
			error = -1;
			e.printStackTrace();
		}
		return error;
	}

	/**
	 * The main function. Use this function for 
	 * testing your code. We will use our own main function for testing.
	 */
	public static void main(String[] args)
	{
		if(args.length != 2)
		{
			System.err.println("Error: Incorrect number of arguments must be <hostname> <portnumber>");
			System.exit(1);
		}
		String host = args[0];
		int port = Integer.parseInt(args[2]); 
		TCPclient client = new TCPclient();
		client.createSocket(host, port);
		client.sendRequest(sendString);
		receivedString = client.receiveResponse();
		client.closeSocket();
	
	}

}
