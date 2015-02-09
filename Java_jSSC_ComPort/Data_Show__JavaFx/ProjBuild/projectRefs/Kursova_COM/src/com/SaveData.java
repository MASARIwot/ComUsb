package com;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.NoSuchElementException;
import java.util.Queue;



public class SaveData{
	
	private volatile static  SaveData instance ;//= new SingletonDoubleCheckedLocking();  //instance-приклад//= new Singleton();
	/*
	 * volatile:
	 * The value of this variable will never be cached thread-locally: all reads and writes will go straight to "main memory";
		Access to the variable acts as though it is enclosed in a synchronized block, synchronized on itself.
	 */
	private SaveData(){
		
	}
	public static SaveData getInstance(){
		if(instance == null){
			synchronized (SaveData.class) {
				if(instance == null) instance = new SaveData();
				
			}
		}
		return instance; // or Singleton.instance
	}
	/*Main Program*/
	StringBuilder buff = new StringBuilder();
	ArrayList<String> ressArr = new ArrayList<String>();
	Queue<String> queue =  new LinkedList<String>();
		
	public  void setData(String str)	{
		
		if(str.contains("s")) { buff = new StringBuilder();} else
		if(str.contains("e")) { buff.append(" "); String str2 = buff.toString(); queue.offer(str2); System.out.println(str2);}
		else buff.append(str);
		
		
	}
	public  String getData() throws NoSuchElementException{
		 if (queue.size() == 0) throw new NoSuchElementException();
		return queue.remove();
	}

}
