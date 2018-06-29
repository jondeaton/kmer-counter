name := "spark-counter"

version := "0.1"

scalaVersion := "2.12.6"
libraryDependencies += "org.scala-lang.modules" %% "scala-parser-combinators" % "1.0.5"
libraryDependencies += "com.github.scopt" %% "scopt" % "3.7.0"
libraryDependencies += "org.apache.spark" %% "spark-core_2.11" % "1.6.1" % "provided"

resolvers += "Cloudera Repository" at "https://repository.cloudera.com/artifactory/cloudera-repos/"
