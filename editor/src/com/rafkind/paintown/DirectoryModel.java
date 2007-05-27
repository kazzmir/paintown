package com.rafkind.paintown;

import java.io.File;
import java.util.List;
import java.util.ArrayList;

import javax.swing.ListModel;
import javax.swing.event.ListDataListener;

public class DirectoryModel implements ListModel {

	private File[] files;
	private List listeners;
	
	public DirectoryModel( String dir ){
		setDirectory( dir );
		listeners = new ArrayList();
	}

	public void setDirectory( String dir ){
		this.files = new File( dir ).listFiles();
	}

	public void addListDataListener( ListDataListener listener ){
		listeners.add( listener );
	}

	public Object getElementAt( int index ){
		return this.files[ index ];
	}

	public int getSize(){
		return this.files.length;
	}

	public void removeListDataListener( ListDataListener listener ){
		listeners.remove( listener );
	}

}
