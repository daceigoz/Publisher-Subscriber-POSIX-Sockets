import java.io.*;
import java.lang.*;
import java.util.*;
import java.net.*;
import gnu.getopt.Getopt;


class suscriptor {

	/********************* TYPES **********************/

	static Socket sc;
	static ObjectOutput s;
	static DataInputStream istream;

	/******************* ATTRIBUTES *******************/

	private static String _server   = null;
	private static short _port = -1;
	private static String action = null;
	static int res;


	/********************* METHODS ********************/

	static int subscribe(String topic)
	{
		// Write your code here
		try{
		sc=new Socket(_server,_port);
		s=new ObjectOutputStream(sc.getOutputStream());
		istream=new DataInputStream(sc.getInputStream());
	}
		catch(Exception e){
		System.out.print("Error in the connection to the server <"+_server+">:<"+_port+">\n");
		System.exit(0);
	}
		try{
			action="SUBSCRIBE\0";
			s.writeObject(action);
			s.flush();
			s.writeObject(topic);
			s.flush();
			res=istream.readInt();
	}
		catch(Exception e){
		System.out.print("NETWORK ERROR\n");
	}
		//0 if OK, 1 if fails
		if(res==0)System.out.println("SUBSCRIBE OK");
		else System.out.println("SUBSCRIBE FAIL");

		try{
		sc.close();
		}
		catch(Exception e){
		}
		return res;
	}

	static int unsubscribe(String topic)
	{
		// Write your code here
		try{
		sc=new Socket(_server,_port);
		s=new ObjectOutputStream(sc.getOutputStream());
		istream=new DataInputStream(sc.getInputStream());
	}
		catch(Exception e){
		System.out.print("Error in the connection to the server <"+_server+">:<"+_port+">\n");
		System.exit(0);
	}
		System.out.println("Got here.");
		try{
			action="UNSUBSCRIBE\0";
			s.writeObject(action);
			s.flush();
			s.writeObject(topic);
			s.flush();
			res=istream.readInt();
	}
		catch(Exception e){
		System.out.print("NETWORK ERROR\n");
	}
		//0 if OK, 1 if topic doesnt exist, 2 if fails
		if(res==0)System.out.println("UNSUBSCRIBE OK");
		else if(res==2) System.out.println("UNSUBSCRIBE FAIL");
		else System.out.println("TOPIC NOT SUBSCRIBED");

		try{
		sc.close();
		}
		catch(Exception e){
		}
		return res;
	}



	/**
	 * @brief Command interpreter for the suscriptor. It calls the protocol functions.
	 */
	static void shell()
	{
		boolean exit = false;
		String input;
		String [] line;
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));

		while (!exit) {
			try {
				System.out.print("c> ");
				input = in.readLine();
				line = input.split("\\s");

				if (line.length > 0) {
					/*********** SUBSCRIBE *************/
					if (line[0].equals("SUBSCRIBE")) {
						if  (line.length == 2) {
							subscribe(line[1]); // topic = line[1]
						} else {
							System.out.println("Syntax error. Usage: SUBSCRIBE <topic>");
						}
					}

					/********** UNSUBSCRIBE ************/
					else if (line[0].equals("UNSUBSCRIBE")) {
						if  (line.length == 2) {
							unsubscribe(line[1]); // topic = line[1]
						} else {
							System.out.println("Syntax error. Usage: UNSUBSCRIBE <topic>");
						}
                    }

                    /************** QUIT **************/
                    else if (line[0].equals("QUIT")){
						if (line.length == 1) {
							exit = true;
						} else {
							System.out.println("Syntax error. Use: QUIT");
						}
					}

					/************* UNKNOWN ************/
					else {
						System.out.println("Error: command '" + line[0] + "' not valid.");
					}
				}
			} catch (java.io.IOException e) {
				System.out.println("Exception: " + e);
				e.printStackTrace();
			}
		}
	}

	/**
	 * @brief Prints program usage
	 */
	static void usage()
	{
		System.out.println("Usage: java -cp . suscriptor -s <server> -p <port>");
	}

	/**
	 * @brief Parses program execution arguments
	 */
	static boolean parseArguments(String [] argv)
	{
		Getopt g = new Getopt("suscriptor", argv, "ds:p:");

		int c;
		String arg;

		while ((c = g.getopt()) != -1) {
			switch(c) {
				//case 'd':
				//	_debug = true;
				//	break;
				case 's':
					_server = g.getOptarg();
					break;
				case 'p':
					arg = g.getOptarg();
					_port = Short.parseShort(arg);
					break;
				case '?':
					System.out.print("getopt() returned " + c + "\n");
					break; // getopt() already printed an error
				default:
					System.out.print("getopt() returned " + c + "\n");
			}
		}

		if (_server == null)
			return false;

		if ((_port < 1024) || (_port > 65535)) {
			System.out.println("Error: Port must be in the range 1024 <= port <= 65535");
			return false;
		}

		return true;
	}



	/********************* MAIN **********************/

	public static void main(String[] argv)
	{
		if(!parseArguments(argv)) {
			usage();
			return;
		}


				// Write code here

		shell();


		System.out.println("+++ FINISHED +++");
	}
}
