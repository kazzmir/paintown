package com.rafkind.paintown;

import java.util.HashMap;
import java.io.*;

/* saves arbitrary data to a file so it can be loaded in between program instances */
public class Config{
    private HashMap<String, Serializable> config = new HashMap<String, Serializable>();
    private static Object lock = new Object();
    private static Config instance = null;
    private static final String savedFile = "editor-config.obj";

    private Config(){
        config = loadData();
    }

    private void saveData(){
        try{
            ObjectOutputStream output = new ObjectOutputStream(new FileOutputStream(savedFile));
            output.writeObject(config);
            output.close();
        } catch (IOException fail){
            System.err.println(fail);
        }
    }

    private HashMap<String, Serializable> loadData(){
        try{
            ObjectInputStream input = new ObjectInputStream(new FileInputStream(savedFile));
            Object in = input.readObject();
            input.close();
            if (in instanceof HashMap){
                return (HashMap<String, Serializable>) in;
            }
            return new HashMap<String, Serializable>();
        } catch (ClassNotFoundException fail){
            return new HashMap<String, Serializable>();
        } catch (IOException fail){
            System.err.println(fail);
            return new HashMap<String, Serializable>();
        }
    }

    public static Config getConfig(){
        synchronized (lock){
            if (instance == null){
                instance = new Config();
            }

            return instance;
        }
    }

    public synchronized Serializable get(String name){
        return config.get(name);
    }

    public synchronized void set(String name, Serializable what){
        config.put(name, what);
    }
}
