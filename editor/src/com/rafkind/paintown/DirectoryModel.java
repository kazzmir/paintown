package com.rafkind.paintown;

import java.io.File;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;

import javax.swing.ListModel;
import javax.swing.event.ListDataListener;
import javax.swing.event.ListDataEvent;

public class DirectoryModel implements ListModel {

	private List files;
	private List listeners;
	
	public DirectoryModel( String dir ){
		listeners = new ArrayList();
		files = new ArrayList();
		setDirectory( dir );
	}

	public void setDirectory( File dir ){
		if ( dir.isDirectory() ){
			files.clear();
			files.add( new File( dir.getPath() + "/.." ) );
			File[] fs = dir.listFiles();
			for ( int i = 0; i < fs.length; i++ ){
				files.add( fs[ i ] );
			}

			ListDataEvent event = new ListDataEvent( this, ListDataEvent.CONTENTS_CHANGED, 0, 999999 );
			for ( Iterator it = listeners.iterator(); it.hasNext(); ){
					ListDataListener l = (ListDataListener) it.next();
					l.contentsChanged( event );
			}
		}
	}

	public void setDirectory( String dir ){
		setDirectory( new File( dir ) );
	}

	public void addListDataListener( ListDataListener listener ){
		listeners.add( listener );
	}

	public Object getElementAt( int index ){
		return this.files.get( index );
	}

	public int getSize(){
		return this.files.size();
	}

	public void removeListDataListener( ListDataListener listener ){
		listeners.remove( listener );
	}

}
