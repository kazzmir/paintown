/*--
 $Id: SwingEngine.java,v 1.5 2005/06/29 08:02:37 wolfpaulus Exp $

 Copyright (C) 2003-2007 Wolf Paulus.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions, and the disclaimer that follows
 these conditions in the documentation and/or other materials provided
 with the distribution.

 3. The end-user documentation included with the redistribution,
 if any, must include the following acknowledgment:
        "This product includes software developed by the
         SWIXML Project (http://www.swixml.org/)."
 Alternately, this acknowledgment may appear in the software itself,
 if and wherever such third-party acknowledgments normally appear.

 4. The name "Swixml" must not be used to endorse or promote products
 derived from this software without prior written permission. For
 written permission, please contact <info_AT_swixml_DOT_org>

 5. Products derived from this software may not be called "Swixml",
 nor may "Swixml" appear in their name, without prior written
 permission from the Swixml Project Management.

 THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED.  IN NO EVENT SHALL THE SWIXML PROJECT OR ITS
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 SUCH DAMAGE.
 ====================================================================

 This software consists of voluntary contributions made by many
 individuals on behalf of the Swixml Project and was originally
 created by Wolf Paulus <wolf_AT_swixml_DOT_org>. For more information
 on the Swixml Project, please see <http://www.swixml.org/>.
*/
package org.swixml;

import javax.swing.*;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;

import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.*;
import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.net.URL;
import java.security.AccessControlException;
import java.util.*;
import java.util.List;

/**
 * The SwingEngine class is the rendering engine able to convert an XML descriptor into a java.swing UI.
 * <p/>
 * <img src="doc-files/swixml_1_0.png" ALIGN="center">
 * </p>
 *
 * @author <a href="mailto:wolf@paulus.com">Wolf Paulus</a>
 * @version $Revision: 1.5 $
 */
@SuppressWarnings("UnusedDeclaration")
public class SwingEngine {
  //
  //  Static Constants
  //
  /**
   * Mac OSX identifier in System.getProperty(os.name)
   */
  public static final String MAC_OSX_OS_NAME = "mac os x";

  /**
   * Mac OSX locale variant to localize strings like quit etc.
   */
  public static final String MAC_OSX_LOCALE_VARIANT = "mac";

  /**
   * XML Error
   */
  private static final String XML_ERROR_MSG = "Invalid SwiXML Descriptor.";
  /**
   * IO Error Message.
   */
  private static final String IO_ERROR_MSG = "Resource could not be found ";
  /**
   * Mapping Error Message.
   */
  private static final String MAPPING_ERROR_MSG = " could not be mapped to any Object and remained un-initialized.";

  //
  //  Static Member Variables
  //

  /**
   * Debug / Release Mode
   */
  public static boolean DEBUG_MODE = false;
  /**
   * main frame
   */
  private static Frame appFrame;
  /**
   * static resource bundle
   */
  private static String default_resource_bundle_name = null;
  /**
   * static locale
   */
  private static Locale default_locale = Locale.getDefault();
  /**
   * Check is currently running on a Mac
   */
  private static boolean MAC_OSX = false;
  /**
   * static Mac OS X Support, set to true to support Mac UI specialties
   */
  private static boolean MAC_OSX_SUPPORTED = true;

  //
  //   Static Initializer
  //
  /** display the swing release version to system out. */
  static {
    System.out.println("SwixML @version@");
    try {
      MAC_OSX = System.getProperty("os.name").toLowerCase().startsWith(SwingEngine.MAC_OSX_OS_NAME);
    } catch (Exception e) {
      MAC_OSX = false;
    }
  }

  //
  //  Member Variables
  //
  /**
   * Swixml Parser.
   */
  private Parser parser = new Parser(this);
  /**
   * Client object hosting the swingengine, alternative to extending the SwinEngine Class
   */
  private Object client;
  /**
   * Root Component for the rendered swing UI.
   */
  private Container root;
  /**
   * Swing object map, contains only those object that were given an id attribute.
   */
  private Map<String, Object> idmap = new HashMap<String, Object>();
  /**
   * Flattened Swing object tree, contains all object, even the ones without an id.
   */
  private Collection components = null;
  /**
   * access to taglib to let overwriting class add and remove tags.
   */
  private Localizer localizer = new Localizer();
  //
  //  Private Constants
  //
  /**
   * Classload to load resources
   */
  private final TagLibrary taglib = SwingTagLibrary.getInstance();
  /**
   * Localizer, setup by parameters found in the xml descriptor.
   */
  protected ClassLoader cl = this.getClass().getClassLoader();

  /**
   * Default ctor for a SwingEngine.
   */
  public SwingEngine() {
    this.client = this;
    this.setLocale(SwingEngine.default_locale);
    this.getLocalizer().setResourceBundle(SwingEngine.default_resource_bundle_name);

    try {
      if (SwingEngine.isMacOSXSupported() && SwingEngine.isMacOSX()) {
        // Use apple's ScreenMenuBar instead of the MS-Window style
        // application's own menu bar
        System.setProperty("com.apple.macos.useScreenMenuBar", "true");
        System.setProperty("apple.laf.useScreenMenuBar", "true");

        // Don't let the growbox intrude other widgets
        System.setProperty("apple.awt.showGrowBox", "true");
        System.setProperty("com.apple.mrj.application.growbox.intrudes", "false");
      }
    } catch (AccessControlException e) {
      // intentionally empty
    }
  }

  /**
   * Constructor to be used if the SwingEngine is not extend but used through object composition.
   *
   * @param client <code>Object</code> owner of this instance
   */
  public SwingEngine(Object client) {
    this();
    this.client = client;
  }

  /**
   * Constructs a new SwingEngine, rendering the provided XML into a javax.swing UI
   *
   * @param resource <code>String</code>
   */
  @SuppressWarnings("deprecation")
  public SwingEngine(final String resource) {
    this(SwingEngine.class.getClassLoader(), resource);
  }

  /**
   * Constructs a new SwingEngine, rendering the provided XML into a javax.swing UI
   *
   * @param resource <code>String</code>
   * @deprecated
   */
  public SwingEngine(ClassLoader cl, final String resource) {
    this();
    this.setClassLoader(cl);
      InputStream in = cl.getResourceAsStream(resource);
     try {
      if (in == null) {
        throw new IOException(IO_ERROR_MSG + resource);
      }
      render(in);
    } catch (Exception e) {
      if (SwingEngine.DEBUG_MODE)
        System.err.println(e);
    } finally {
      try {
          if (in!=null) {
              in.close();
          }
      } catch (Exception e) {
        // intentionally empty
      }
    }
  }

  /**
   * Gets the parsing of the XML started.
   *
   * @param url <code>URL</code> url pointing to an XML descriptor
   * @return <code>Object</code>- instanced swing object tree root
   * @throws Exception
   */
  public Container render(final URL url) throws Exception {
    Container obj = null;
      InputStream in = url.openStream();
      if (in == null) {
        throw new IOException(IO_ERROR_MSG + url.toString());
      }
    try {
      obj = render(in);
    } finally {
      try {
        in.close();
      } catch (Exception ex) {
        // intentionally empty
      }
    }
    return obj;
  }

  /**
   * Gets the parsing of the XML file started.
   *
   * @param resource <code>String</code> xml-file path info
   * @return <code>Object</code>- instanced swing object tree root
   */
  public Container render(final String resource) throws Exception {
    Container obj = null;
      InputStream in = cl.getResourceAsStream(resource);
      if (in == null) {
        throw new IOException(IO_ERROR_MSG + resource);
      }
    try {
      obj = render(in);
    } finally {
      try {
        in.close();
      } catch (Exception ex) {
        // intentionally empty
      }
    }
    return obj;
  }

  /**
   * Gets the parsing of the XML file started.
   *
   * @param xml_file <code>File</code> xml-file
   * @return <code>Object</code>- instanced swing object tree root
   */
  public Container render(final File xml_file) throws Exception {
    if (xml_file == null) {
      throw new IOException();
    }
    FileInputStream in=new FileInputStream(xml_file);
    try {
        return render(in);
    } finally {
        try {
          in.close();
        } catch (Exception ex) {
            // intentionally empty
        }
    }
  }

  /**
   * Gets the parsing of the XML file started.
   *
   * @param xml_reader <code>Reader</code> xml-file path info
   * @return <code>Object</code>- instanced swing object tree root
   */
  public Container render(final InputStream xml_reader) throws Exception {
    if (xml_reader == null) {
      throw new IOException();
    }
    try {
      DocumentBuilder builder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
      return render(builder.parse(xml_reader));
    } catch (Exception e) {
      System.err.println(e);
    }
    throw new Exception(SwingEngine.XML_ERROR_MSG);
  }

  /**
   * Gets the parsing of the XML file started.
   *
   * @param jdoc <code>Document</code> xml gui descritptor
   * @return <code>Object</code>- instanced swing object tree root
   */
  public Container render(final Document jdoc) throws Exception {
    idmap.clear();
    try {
      root = (Container) parser.parse(jdoc);
    } catch (Exception e) {
      if (SwingEngine.DEBUG_MODE)
        System.err.println(e);
      throw (e);
    }
    // reset components collection
    components = null;
    // initialize all client fields with UI components by their id
    mapMembers(client);
    if (Frame.class.isAssignableFrom(root.getClass())) {
      SwingEngine.setAppFrame((Frame) root);
    }
    return root;
  }

  /**
   * Inserts swing object rendered from an XML document into the given container.
   * <p/>
   * <pre>
   *  Differently to the render methods, insert does NOT consider the root node of the XML document.
   * </pre>
   * <pre>
   *  <b>NOTE:</b><br>insert() does NOT clear() the idmap before rendering.
   * Therefore, if this SwingEngine's parser was used before, the idmap still
   * contains (key/value) pairs (id, JComponent obj. references).<br>If insert() is NOT
   * used to insert in a previously (with this very SwingEngine) rendered UI,
   * it is highly recommended to clear the idmap:
   * <br>
   *  <div>
   *    <code>mySwingEngine.getIdMap().clear()</code>
   *  </div>
   * </pre>
   *
   * @param url       <code>URL</code> url pointing to an XML descriptor *
   * @param container <code>Container</code> target, the swing obj, are added to.
   * @throws Exception
   */
  public void insert(final URL url, final Container container)
          throws Exception {
      InputStream in = url.openStream();
      if (in == null) {
        throw new IOException(IO_ERROR_MSG + url.toString());
      }
    try {
      insert(in, container);
    } finally {
      try {
        in.close();
      } catch (Exception ex) {
        // intentionally empty
      }
    }
  }

  /**
   * Inserts swing objects rendered from an XML reader into the given container.
   * <p/>
   * <pre>
   *  Differently to the render methods, insert does NOT consider the root node of the XML document.
   * </pre>
   * <pre>
   *  <b>NOTE:</b><br>insert() does NOT clear() the idmap before rendering.
   * Therefore, if this SwingEngine's parser was used before, the idmap still
   * contains (key/value) pairs (id, JComponent obj. references).<br>If insert() is NOT
   * used to insert in a previously (with this very SwingEngine) rendered UI, it is highly
   * recommended to clear the idmap:
   * <br>
   *  <div>
   *    <code>mySwingEngine.getIdMap().clear()</code>
   *  </div>
   * </pre>
   *
   * @param reader    <code>Reader</code> xml-file path info
   * @param container <code>Container</code> target, the swing obj, are added to.
   * @throws Exception
   */
  public void insert(final InputStream reader, final Container container)
          throws Exception {
    if (reader == null) {
      throw new IOException();
    }
    DocumentBuilder builder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
    insert(builder.parse(reader), container);
  }

  /**
   * Inserts swing objects rendered from an XML reader into the given container.
   * <p/>
   * <pre>
   *  Differently to the render methods, insert does NOT consider the root node of the XML document.
   * </pre>
   * <pre>
   *  <b>NOTE:</b><br>insert() does NOT clear() the idmap before rendering.
   * Therefore, if this SwingEngine's parser was used before, the idmap still
   * contains (key/value) pairs (id, JComponent obj. references).<br>
   * If insert() is NOT used to insert in a previously (with this very SwingEngine)
   * rendered UI, it is highly recommended to clear the idmap:
   * <br>
   *  <div>
   *    <code>mySwingEngine.getIdMap().clear()</code>
   *  </div>
   * </pre>
   *
   * @param resource  <code>String</code> xml-file path info
   * @param container <code>Container</code> target, the swing obj, are added to.
   * @throws Exception
   */
  public void insert(final String resource, final Container container) throws Exception {
      InputStream in = cl.getResourceAsStream(resource);
      if (in == null) {
        throw new IOException(IO_ERROR_MSG + resource);
      }
    try {
      insert(in, container);
    } finally {
      try {
          in.close();
      } catch (Exception ex) {
        // intentionally empty
      }
    }
  }

  /**
   * Inserts swing objects rendered from an XML document into the given container.
   * <p/>
   * <pre>
   *  Differently to the parse methods, insert does NOT consider the root node of the XML document.
   * </pre>
   * <pre>
   *  <b>NOTE:</b><br>insert() does NOT clear() the idmap before rendering.
   * Therefore, if this SwingEngine's parser was used before, the idmap still
   * contains (key/value) pairs (id, JComponent obj. references).<br>
   * If insert() is NOT
   * used to insert in a previously (with this very SwingEngine) rendered UI,
   * it is highly recommended to clear the idmap:
   * <br>
   *  <div>
   *    <code>mySwingEngine.getIdMap().clear()</code>
   *  </div>
   * </pre>
   *
   * @param jdoc      <code>Document</code> xml-doc path info
   * @param container <code>Container</code> target, the swing obj, are added to
   * @throws Exception <code>Exception</code> exception thrown by the parser
   */
  public void insert(final Document jdoc, final Container container)
          throws Exception {
    root = container;
    try {
      parser.parse(jdoc, container);
    } catch (Exception e) {
      if (SwingEngine.DEBUG_MODE)
        System.err.println(e);
      throw (e);
    }
    // reset components collection
    components = null;
    // initialize all client fields with UI components by their id
    mapMembers(client);
  }

  /**
   * Sets the SwingEngine's global resource bundle name, to be used by all SwingEngine instances. This name can be
   * overwritten however for a single instance, if a <code>bundle</code> attribute is places in the root tag of an XML
   * descriptor.
   *
   * @param bundlename <code>String</code> the resource bundle name.
   */
  public static void setResourceBundleName(String bundlename) {
    SwingEngine.default_resource_bundle_name = bundlename;
  }

  /**
   * Sets the SwingEngine's global locale, to be used by all SwingEngine instances. This locale can be overwritten
   * however for a single instance, if a <code>locale</code> attribute is places in the root tag of an XML descriptor.
   *
   * @param locale <code>Locale</code>
   */
  public static void setDefaultLocale(Locale locale) {
    SwingEngine.default_locale = locale;
  }

  /**
   * Sets the SwingEngine's global application frame variable, to be used as a parent for all child dialogs.
   *
   * @param frame <code>Window</code> the parent for all future dialogs.
   */
  public static void setAppFrame(Frame frame) {
    if (frame != null) {
      if (SwingEngine.appFrame == null) {
        SwingEngine.appFrame = frame;
      }
    }
  }

  /**
   * @return <code>Window</code> a parent for all dialogs.
   */
  public static Frame getAppFrame() {
    return SwingEngine.appFrame;
  }

  /**
   * Returns the object which instantiated this SwingEngine.
   *
   * @return <code>Objecy</code> SwingEngine client object
   *         <p/>
   *         <p><b>Note:</b><br>
   *         This is the object used through introspection the actions and fileds are set.
   *         </p>
   */
  public Object getClient() {
    return client;
  }

  /**
   * Returns the root component of the generated Swing UI.
   *
   * @return <code>Component</code>- the root component of the javax.swing ui
   */
  public Container getRootComponent() {
    return root;
  }

  /**
   * Returns an Iterator for all parsed GUI components.
   *
   * @return <code>Iterator</code> GUI components itearator
   */
  public Iterator getAllComponentItertor() {
    if (components == null) {
      traverse(root, components = new ArrayList());
    }
    return components.iterator();
  }

  /**
   * Returns an Iterator for id-ed parsed GUI components.
   *
   * @return <code>Iterator</code> GUI components itearator
   */
  public Iterator getIdComponentItertor() {
    return idmap.values().iterator();
  }

  /**
   * Returns the id map, containing all id-ed parsed GUI components.
   *
   * @return <code>Map</code> GUI components map
   */
  public Map<String, Object> getIdMap() {
    return idmap;
  }

  /**
   * Removes all un-displayable compontents from the id map and deletes the components collection (for recreation at the
   * next request).
   * <p/>
   * <pre>
   *  A component is made undisplayable either when it is removed from a displayable containment hierarchy or when its
   * containment hierarchy is made undisplayable. A containment hierarchy is made undisplayable when its ancestor
   * window
   * is disposed.
   * </pre>
   *
   * @return <code>int</code> number of removed componentes.
   */
  public int cleanup() {
    List zombies = new ArrayList();
    Iterator it = idmap.keySet().iterator();
    while (it != null && it.hasNext()) {
      Object key = it.next();
      Object obj = idmap.get(key);
      if (obj instanceof Component && !((Component) obj).isDisplayable()) {
        zombies.add(key);
      }
    }
    for (int i = 0; i < zombies.size(); i++) {
      idmap.remove(zombies.get(i));
    }
    components = null;
    return zombies.size();
  }

  /**
   * Removes the id from the internal from the id map, to make the given id available for re-use.
   *
   * @param id <code>String</code> assigned name
   */
  public void forget(final String id) {
    idmap.remove(id);
  }

  /**
   * Returns the UI component with the given name or null.
   *
   * @param id <code>String</code> assigned name
   * @return <code>Component</code>- the GUI component with the given name or null if not found.
   */
  public Component find(final String id) {
    Object obj = idmap.get(id);
    if (obj != null && !Component.class.isAssignableFrom(obj.getClass())) {
      obj = null;
    }
    return (Component) obj;
  }

  /**
   * Sets the locale to be used during parsing / String conversion
   *
   * @param l <code>Locale</code>
   */
  public void setLocale(Locale l) {
    if (SwingEngine.isMacOSXSupported() && SwingEngine.isMacOSX()) {
      l = new Locale(l.getLanguage(),
              l.getCountry(),
              SwingEngine.MAC_OSX_LOCALE_VARIANT);
    }
    this.localizer.setLocale(l);
  }

  /**
   * Sets the ResourceBundle to be used during parsing / String conversion
   *
   * @param bundlename <code>String</code>
   */
  public void setResourceBundle(String bundlename) {
    this.localizer.setResourceBundle(bundlename);
  }

  /**
   * @return <code>Localizer</code>- the Localizer, which is used for localization.
   */
  public Localizer getLocalizer() {
    return localizer;
  }

  /**
   * @return <code>TagLibrary</code>- the Taglibray to insert custom tags.
   *         <p/>
   *         <pre><b>Note:</b>ConverterLibrary and TagLibray need to be set up before rendering is called.
   *                                                                                                                                                                 </pre>
   */
  public TagLibrary getTaglib() {
    return taglib;
  }

  /**
   * Sets a classloader to be used for all <i>getResourse..()</i> and <i> loadClass()</i> calls. If no class loader is
   * set, the SwingEngine's loader is used.
   *
   * @param cl <code>ClassLoader</code>
   * @see ClassLoader#loadClass
   * @see ClassLoader#getResource
   */
  public void setClassLoader(ClassLoader cl) {
    this.cl = cl;
    this.localizer.setClassLoader(cl);
  }

  /**
   * @return <code>ClassLoader</code>- the Classloader used for all <i> getResourse..()</i> and <i>loadClass()</i>
   *         calls.
   */
  public ClassLoader getClassLoader() {
    return cl;
  }

  /**
   * Recursively Sets an ActionListener
   * <p/>
   * <pre>
   *  Backtracking algorithm: if al was set for a child component, its not being set for its parent
   * </pre>.
   *
   * @param c  <code>Component</code> start component
   * @param al <code>ActionListener</code>
   * @return <code>boolean</code> true, if ActionListener was set.
   */
  public boolean setActionListener(final Component c, final ActionListener al) {
    boolean b = false;
    if (c != null) {
      if (Container.class.isAssignableFrom(c.getClass())) {
        final Component[] s = ((Container) c).getComponents();
        for (Component value : s) {
          b = b | setActionListener(value, al);
        }
      }
      if (!b) {
        if (JMenu.class.isAssignableFrom(c.getClass())) {
          final JMenu m = (JMenu) c;
          final int k = m.getItemCount();
          for (int i = 0; i < k; i++) {
            b = b | setActionListener(m.getItem(i), al);
          }
        } else if (AbstractButton.class.isAssignableFrom(c.getClass())) {
          ((AbstractButton) c).addActionListener(al);
          b = true;
        }
      }

    }
    return b;
  }

  /**
   * Walks the whole tree to add all components into the <code>components<code> collection.
   *
   * @param c <code> Component</code> recursive start component.
   *          <p/>
   *          Note:There is another collection available that only tracks
   *          those object that were provided with an <em>id</em>attribute, which hold an unique id
   *          </p>
   * @return <code>Iterator</code> to walk all components, not just the id components.
   */
  public Iterator getDescendants(final Component c) {
    List list = new ArrayList(12);
    SwingEngine.traverse(c, list);
    return list.iterator();
  }

  /**
   * Introspects the given object's class and initializes its non-transient fields with objects that have been instanced
   * during parsing. Mappping happens based on type and field name: the fields name has to be equal to the tag id,
   * psecified in the XML descriptor. The fields class has to be assignable (equals or super class..) from the class
   * that was used to instance the tag.
   *
   * @param obj <code>Object</code> target object to be mapped with instanced tags
   */
  protected void mapMembers(Object obj) {
    if (obj != null) {
      mapMembers(obj, obj.getClass());
    }
  }

  private void mapMembers(Object obj, Class cls) {
    boolean fullaccess = true;

    if (obj != null && cls != null && !Object.class.equals(cls)) {
      Field[] flds = null;
      try {
        flds = cls.getDeclaredFields();
      } catch (AccessControlException e) {
        fullaccess = false; // applet or otherwise restricted environment
        flds = cls.getFields();
      }
      //
      // loops through class' declared fields and try to find a matching widget.
      //
      for (int i = 0; i < flds.length; i++) {
        Object widget = idmap.get(flds[i].getName());
        if (widget != null) {
          // field and object type need to be compatible and field must not be declared Transient
          if (flds[i].getType().isAssignableFrom(widget.getClass()) && !Modifier.isTransient(flds[i].getModifiers())) {
            try {

              boolean accessible = flds[i].isAccessible();
              flds[i].setAccessible(true);
              flds[i].set(obj, widget);
              flds[i].setAccessible(accessible);
            } catch (AccessControlException e) {
              try {
                fullaccess = false;
                flds[i].set(obj, widget);
              } catch (IllegalAccessException e1) {
              }

            } catch (IllegalArgumentException e) {
              // intentionally empty
            } catch (IllegalAccessException e) {
              // intentionally empty
            }
          }
        }

        //
        //  If an intended mapping didn't work out the objects member would remain un-initialized.
        //  To prevent this, we try to instantiate with a default ctor.
        //
        if (flds[i] == null) {
          if (!SwingEngine.DEBUG_MODE) {
            try {
              flds[i].set(obj, flds[i].getType().newInstance());
            } catch (IllegalArgumentException e) {
              // intentionally empty
            } catch (IllegalAccessException e) {
              // intentionally empty
            } catch (InstantiationException e) {
              // intentionally empty
            }
          } else { // SwingEngine.DEBUG_MODE)
            System.err.println(flds[i].getType()
                    + " : "
                    + flds[i].getName()
                    + SwingEngine.MAPPING_ERROR_MSG);
          }
        }
      }

      // Since getDeclaredFields() only works on the class itself, not the super class,
      // we need to make this recursive down to the object.class
      if (fullaccess) {
        // only if we have access to the declared fields do we need to visit the whole tree.
        mapMembers(obj, cls.getSuperclass());
      }
    }
  }

  /**
   * Walks the whole tree to add all components into the <code>components<code> collection.
   *
   * @param c          <code>Component</code> recursive start component.
   * @param collection <code>Collection</code> target collection.
   *                   <p/>
   *                   Note:There is another collection available that only tracks
   *                   those object that were provided with an <em>id</em>attribute, which hold an unique id
   *                   </p>
   */
  protected static void traverse(final Component c, Collection collection) {
    if (c != null) {
      collection.add(c);
      if (c instanceof JMenu) {
        final JMenu m = (JMenu) c;
        final int k = m.getItemCount();
        for (int i = 0; i < k; i++) {
          traverse(m.getItem(i), collection);
        }
      } else if (c instanceof Container) {
        final Component[] s = ((Container) c).getComponents();
        for (Component value : s) {
          traverse(value, collection);
        }
      }
    }
  }

  /**
   * Enables or disables support of Mac OS X GUIs
   *
   * @param osx <code>boolean</code>
   */
  public static void setMacOSXSuport(boolean osx) {
    SwingEngine.MAC_OSX_SUPPORTED = osx;
  }

  /**
   * Indicates state of Mac OS X support (default is true = ON).
   *
   * @return <code>boolean</code>- indicating MacOS support is enabled
   */
  public static boolean isMacOSXSupported() {
    return SwingEngine.MAC_OSX_SUPPORTED;
  }

  /**
   * Indicates if currently running on Mac OS X
   *
   * @return <code>boolean</code>- indicating if currently running on a MAC
   */
  public static boolean isMacOSX() {
    return SwingEngine.MAC_OSX;
  }

  /**
   * Displays the GUI during a RAD session. If the root component is neither a JFrame nor a JDialog, the a JFrame is
   * instantiated and the root is added into the new frames contentpane.
   */
  public void test() {
    WindowListener wl = new WindowAdapter() {
      public void windowClosing(WindowEvent e) {
        super.windowClosing(e);
        System.exit(0);
      }
    };
    if (root != null) {
      if (JFrame.class.isAssignableFrom(root.getClass())
              || JDialog.class.isAssignableFrom(root.getClass())) {
        ((Window) root).addWindowListener(wl);
        root.setVisible(true);
      } else {
        JFrame jf = new JFrame("SwiXml Test");
        jf.getContentPane().add(root);
        jf.pack();
        jf.addWindowListener(wl);
        jf.setVisible(true);
      }
    }
  }
}
