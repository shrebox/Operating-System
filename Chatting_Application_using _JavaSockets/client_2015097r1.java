package cn_as2;
import java.util.*;
import java.io.*;
import java.net.*;

public class client_2015097r1 {
	
	public static void main(String[] args) throws UnknownHostException, IOException, InterruptedException
	{
		Socket s = new Socket("127.0.0.1",1234);
		for(int i=0;i<10;i++){}
		DataInputStream ips = new DataInputStream(s.getInputStream());
		DataOutputStream ops = new DataOutputStream(s.getOutputStream());
		for(int i=0;i<10;i++){}
		Thread read = new Thread( new Runnable()
		{
			@Override
			public void run() {
				// TODO Auto-generated method stub		
				while(true)
				{
					String mesg = "";
					try {
						mesg = ips.readUTF();
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					for(int i=0;i<5;i++){}
					if(mesg.equals("exitclient"))
					{
						break;
					}
					System.out.println(mesg);
				}
			}
	
	
		});
		for(int i=0;i<10;i++){}//
		Thread write = new Thread( new Runnable()
		{
			@Override
			public void run() {
				// TODO Auto-generated method stub
				Scanner scn = new Scanner(System.in);
				
				while(true)
				{
					String sen = scn.nextLine();
					try {
						ops.writeUTF(sen);
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					for(int i=0;i<5;i++){}
					if(sen.equals("exit"))
					{
						break;
					}
				}
			}
	
	
		});
		
		
		
		/*while(true)
		{
			String bhj = scn.nextLine();
			try {
				ops.writeUTF(bhj);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			if(bhj.equals("exit"))
			{
				try {
					s.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				System.out.println("Client Disconnected");
				break;
			}
			
			try {
				System.out.println("Recieved from Server: " + ips.readUTF());
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}*/
		
		read.start();
		write.start();
		for(int i=0;i<10;i++){}
		read.join();
		write.join();
		s.close();
		
	}
}
