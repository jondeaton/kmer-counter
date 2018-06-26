package kmercounter

import scala.math._
import java.io.PrintWriter

object KmerCounter {

  case class CountRecord(id: String, counts: Map[String, Int])
  private val charMap = Map('A' -> 0,
    'T' -> 1,
    'G' -> 2,
    'C' -> 4)

  def main(args: Array[String]): Unit = {

//    val fasta_file = "~/Datasets/viruses/all_phage_genomes.fasta"

    val fasta_file = "/Users/jonpdeaton/Developer/kmer-counter/test/multiple.fasta"
//    val fasta_file = "/home/jdeaton/repos/kmer-counter/test/multiple.fasta"
    val fn = fasta_file.replaceFirst("~", System.getProperty("user.home"))

    val p = new FastaParser()
    val k = 4

    val fasta = p.fromFile(fasta_file)
    val count_records = fasta.map(e => CountRecord(e.id, count(e.sequence, k)))
    writeCounts(count_records, "out.kmer")
  }

  /** Counts occurences of all sub-sequences of length `k` in `sequence`
    *
    * @param sequence Sequence to count k-mers in
    * @param k The length of each kmer
    * @return
    */
  def count (sequence: String, k: Int): Map[String, Int] = {
    sequence.sliding(k).toTraversable.view.groupBy(identity).mapValues(_.size)
  }

  private kmerIndex (kmer: String, k: Int): Int = {
    kmer.map(c => charMap(c))
    
  }

  def writeCounts(m: List[CountRecord], file_name: String): Unit = {
    val pw = new PrintWriter(file_name)
    m.foreach { writeCount(_, pw) }
    pw.close()
  }

  private def writeCount(m: CountRecord, pw: PrintWriter): Unit = {
    pw.write(s"""> ${m.id}, """)
    pw.write(m.counts.toList.sortBy(tuple => tuple._2).toString)
    pw.write("\n")
  }
}