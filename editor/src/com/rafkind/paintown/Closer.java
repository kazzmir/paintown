package com.rafkind.paintown;

public abstract class Closer{
    private static int opened = 0;
    
    public static void open(){
        opened += 1;
    }

    public static void close(){
        opened -= 1;
        if (opened <= 0){
            System.exit(0);
        }
    }
}
