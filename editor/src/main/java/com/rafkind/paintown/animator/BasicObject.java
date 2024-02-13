package com.rafkind.paintown.animator;

import java.io.File;

import com.rafkind.paintown.exception.LoadException;

public abstract class BasicObject{
	
	private File path;

	private String name = "";

	public BasicObject( String name ){
		this.name = name;
	}

	public void setPath(File f){
		path = f;
	}

	public File getPath(){
		return path;
	}

	public String getName(){
		return name;
	}
	
	public void setName(String n){
		name = n;
	}
	
	abstract public void saveData() throws LoadException;
	abstract public void loadData( File f ) throws LoadException;
}
