package kmercounter

import java.io.File
import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.SparkConf

class SparkKmerCounter {

  val fastaParser = new FastaParser
  val kmerCounter = new KmerCounter

  /** Counts k-mers in the input file/directory using Apache Spark
    *
    * @param in Input file or directory that contains FASTA records
    * @param out Output file to store k-mer counts
    * @param k Length of k-mer to count in each sequence
    */
  def countFile(in: File, out: File, k: Int): Unit = {
    if (!in.exists || !out.exists) return

    val sc = new SparkContext(new SparkConf().setAppName("k-mer counter"))

    {
      if (in.isFile)
        sc.wholeTextFiles(in.getAbsolutePath)
      else if (in.isDirectory)
        sc.wholeTextFiles(in.listFiles().filter(_.isFile).map(_.getAbsolutePath).mkString(","))
      else return
    }
      .flatMap { case (fn: String, records: String) => fastaParser.fromString(records) }
      .map(record => (record.id, kmerCounter.count(record.sequence, k)))
      .saveAsTextFile(out.getAbsolutePath)
  }
}

