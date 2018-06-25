package KmerCounter

import scala.math._

object KmerCounter {

  def main(args: Array[String]): Unit = {

//    val fasta_file = "~/Datasets/viruses/all_phage_genomes.fasta"

    val fasta_file = "/home/jdeaton/repos/kmer-counter/test/multiple.fasta"
    val fn = fasta_file.replaceFirst("~", System.getProperty("user.home"))

    val p = new FastaParser()
    val k = 4

    println(p.fromFile(fn))

    p.fromFile(fasta_file).map(e => count(e, k)).toStream
  }

  def count (record: FastaRecord, k: Int): Map[String, Int] = {
    record.sequence.sliding(k).toList.groupBy(identity).mapValues(_.size)
  }
}