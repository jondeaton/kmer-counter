package kmercounter

import java.io.{File, FileNotFoundException}
import org.apache.spark.{SparkContext,SparkConf}

object SparkKmerCounter {

  /**
    * Counts k-mers in the input file/directory using Apache Spark.
    * @param in Input file or directory that contains FASTA records
    * @param out Output file to store k-mer counts
    * @param k Length of k-mer to count in each sequence
    */
  def countFile(in: File, out: File, k: Int): Unit = {
    if (!in.exists) throw new FileNotFoundException(in.getAbsolutePath)

    println("Initializing SparkContext...")
    val sc = new SparkContext(new SparkConf().setAppName("k-mer counter"))
    println("Initialized Spark.")

    {
      if (in.isFile)
        sc.wholeTextFiles(in.getPath)
      else if (in.isDirectory)
        sc.wholeTextFiles(in.listFiles().filter(_.isFile).map(_.getAbsolutePath).mkString(","))
      else return
    }
      .values.flatMap(records => FastaParser.fromString(records))
      .map(record => CountRecord(record.id, KmerCounter.count(record.sequence, k)))
      .saveAsTextFile(out.getPath)
  }
}

