package com.rafkind.paintown.level;

import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;
import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;

import javax.swing.AbstractAction;

public abstract class RandomNameAction extends AbstractAction {
	private static HashMap files = new HashMap();
	private List names;

	private List readFile( BufferedReader reader ) throws IOException {
		List l = new ArrayList();
		while ( reader.ready() ){
			l.add( reader.readLine() );
		}
		return l;
	}

	public RandomNameAction( String file ){
		if ( files.get( file ) == null ){
			try{
				files.put( file, readFile( new BufferedReader( new InputStreamReader( this.getClass().getResourceAsStream( "/" + file ) ) ) ) );
				names = (List) files.get( file );
			} catch ( IOException ie ){
				ie.printStackTrace();
				names = new ArrayList();
				names.add( "TempName" );
			}
		} else {
			names = (List) files.get( file );
		}
	}

	public String generateName(){
		return (String) names.get( (int)(Math.random() * (names.size() - 1)) );
	}
}
