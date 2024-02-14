package com.rafkind.paintown.animator;

import java.io.File;
import java.io.PrintStream;
import java.io.FileOutputStream;

import java.util.Iterator;
import java.util.ArrayList;

import com.rafkind.paintown.Lambda1;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.TokenReader;

import com.rafkind.paintown.exception.LoadException;

public class Projectile extends AnimatedObject {

	private Animation main;
	private Animation death;

	public Projectile(String name){
		super(name);

		main = new Animation( "main" );
		new Thread(main).start();
		death = new Animation( "death" );
		new Thread(death).start();

		addAnimation(main);
		addAnimation(death);
	}

	public Projectile(String name, File f) throws LoadException {
		super(name);
		loadData(f);
	}
	
	public Animation getMain(){
		return main;
	}

	public void setMap( int map ){
		Lambda1.foreach_( new ArrayList( getAnimations() ), new Lambda1(){
			public Object invoke( Object a ){
				Animation ani = (Animation) a;
				ani.setMap( null );
				return null;
			}
		});
	}

	public Animation getDeath(){
		return death;
	}
	
	public void saveData() throws LoadException {
		try{
			FileOutputStream out = new FileOutputStream(getPath());
			PrintStream printer = new PrintStream(out);
			printer.print(getToken().toString());
			printer.print("\n");
			out.close();
			System.out.println(getToken().toString());
		} catch ( Exception e ){
			throw new LoadException( "Couldn't save!" );
		}
	}

	public void loadData( File f ) throws LoadException {
		TokenReader reader = new TokenReader( f );
		Token head = reader.nextToken();
		
		if ( ! head.getName().equals( "projectile" ) ){
			throw new LoadException( "Starting token is not 'projectile'" );
		}

		for ( Iterator it = head.findTokens( "anim" ).iterator(); it.hasNext(); ){
			Token animation = (Token) it.next();
			Token name = animation.findToken( "name" );
			if ( name != null ){
				if ( name.readString( 0 ).equals( "main" ) ){
					main = new Animation( animation );
					addAnimation(main);
					new Thread(main).start();
				} else if ( name.readString( 0 ).equals( "death" ) ){
					death = new Animation( animation );
					addAnimation(death);
					new Thread(death).start();
				} else {
					System.out.println("Unknown animation");
					System.out.println(animation.toString());
				}
			}
		}
	}

	public Token getToken(){
		Token temp = new Token("projectile");
		temp.addToken(new Token("projectile"));
		
		Iterator animItor = getAnimations().iterator();
		while(animItor.hasNext()){
			Animation anim = (Animation)animItor.next();
			temp.addToken(anim.getToken());
		}
		
		return temp;
	}
}
