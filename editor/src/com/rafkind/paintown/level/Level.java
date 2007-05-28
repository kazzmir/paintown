package com.rafkind.paintown.level;

import java.awt.*;

public class Level{

	private String name;

	public Level(){
		this.name = "Hellooooo nurse!";
	}

	public void render( Graphics g, int x, int y, int width, int height ){
		g.clearRect( x, y, width, height );
		g.setColor( new Color( 255, 0, 0 ) );
		g.drawString( this.name, 20, 20 );
		g.setColor( new Color( 0, 255, 0 ) );
		g.fillOval( 50, 50, 50, 50 );
	}

}
