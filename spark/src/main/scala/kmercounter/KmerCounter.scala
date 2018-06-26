package kmercounter

import scala.math._
import java.io.PrintWriter

object KmerCounter {

  case class CountRecord(id: String, counts: Array[Int])

  private val charMap = Map('A' -> 0, 'T' -> 1, 'G' -> 2, 'C' -> 3)
  private val sigs = for (i <- List.range(0, 4)) yield pow(4, i).toInt

  def main(args: Array[String]): Unit = {

    val fasta_file = "~/Datasets/viruses/all_phage_genomes.fasta"

//    val fasta_file = "/Users/jonpdeaton/Developer/kmer-counter/test/multiple.fasta"
//    val fasta_file = "/home/jdeaton/repos/kmer-counter/test/multiple.fasta"
    val fn = fasta_file.replaceFirst("~", System.getProperty("user.home"))

    println(fn)
    val p = new FastaParser()
    val k = 4

    println("Loading file...")
    val fasta = p.fromFile(fn)

    println("Counting...")
    val count_records = fasta.par.map {
      e => CountRecord(e.id, count(e.sequence.toUpperCase, k))
    }.toList
    println("Writing...")
    writeCounts(count_records, "out.kmer")
  }

  /** Counts occurences of all sub-sequences of length `k` in `sequence`
    *
    * @param sequence Sequence to count k-mers in
    * @param k The length of each kmer
    * @return
    */
  def count (sequence: String, k: Int): Array[Int] = {
    val arr = new Array[Int](pow(4, k).toInt)
    sequence.sliding(k).foreach { kmer =>
      val index = kmerIndex(kmer, k)
      if (index >= 0) arr(index) += 1
    }
    arr
  }

  /** Calculate the lexicographic index of the kmer
    *
    * @param kmer
    * @param k
    * @return
    */
  private def kmerIndex (kmer: String, k: Int): Int = {
    try {
      kmer.map(charMap).reverse
        .zip(sigs)
        .foldLeft(0) { (acc, t) => acc + t._1 * t._2 }
    } catch {
      case nse: java.util.NoSuchElementException => -1
    }
  }

  def writeCounts(m: List[CountRecord], file_name: String): Unit = {
    val pw = new PrintWriter(file_name)
    m.foreach { writeCount(_, pw) }
    pw.close()
  }

  private def writeCount(m: CountRecord, pw: PrintWriter): Unit = {
    pw.write(s">${m.id},${m.counts.mkString(",")}\n")
  }
}