name := "Paintown Editor"

version := "1.0"

scalaVersion := "2.10.3"

// sourceDirectories in Compile += file("src/MasterOfMagic/src")

// scalaSource in Compile <<= baseDirectory(_/"src/MasterOfMagic/src")

// scalaSource in Compile <<= baseDirectory(_/"src/MasterOfMagic/src")

unmanagedSourceDirectories in Compile <+= baseDirectory(_/"src")

// unmanagedSourceDirectories in Compile <+= baseDirectory(_/"src/Util/src")

