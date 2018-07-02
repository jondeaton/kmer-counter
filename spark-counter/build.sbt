name := "spark-counter"

version := "0.1"

scalaVersion := "2.11.8"

resolvers ++= Seq("apache-snapshots" at "http://repository.apache.org/snapshots/")
libraryDependencies += "com.github.scopt" %% "scopt" % "3.7.0"
libraryDependencies += "org.apache.spark" %% "spark-core" % "2.3.1" % "provided"
