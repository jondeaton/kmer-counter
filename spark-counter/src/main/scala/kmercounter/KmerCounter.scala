package kmercounter

import scala.math._
import java.io.PrintWriter
import java.io.File

case class CountRecord(id: String, counts: Array[Int])

class KmerCounter {

  private val charMap = Map('A' -> 0, 'T' -> 1, 'G' -> 2, 'C' -> 3)
  private val sigs = for (i <- List.range(0, 4)) yield pow(4, i).toInt

  def countFile(in: File, out: File, k: Int): Unit = {
    if (in.isDirectory) _countDirectory(in, out, k)
    else if (in.isFile) _countFile(in, out, k)
  }

  private def _countFile(in: File, out: File, k: Int): Unit = {
    if (in.exists && in.isFile) {
      val p = new FastaParser()
      val fasta = p.fromFile(in)
      val count_records = fasta.par.map {
        e => CountRecord(e.id, count(e.sequence.toUpperCase, k))
      }.toList
      writeCounts(count_records, "out.kmer")
    }
  }

  private def _countDirectory(in: File, out: File, k: Int): Unit =
    if (in.exists && in.isDirectory)
      in.listFiles().filter(_.isFile).foreach(f => _countFile(f, out, k))

  /**
    * Counts occurrences of all sub-sequences of length `k` in `sequence`
    * @param sequence Sequence to count k-mers in
    * @param k The length of each k-mer
    * @return Array containing the counts of each k-mer in `sequence`
    */
  def count(sequence: String, k: Int): Array[Int] = {
    val arr = new Array[Int](pow(4, k).toInt)
    sequence.sliding(k).foreach { kmer =>
      val index = kmerIndex(kmer, k)
      if (index >= 0) arr(index) += 1
    }
    arr
  }

  /**
    * Calculate the lexicographic index of the k-mer
    * @param kmer The k-mer to get the index of
    * @param k The length of the k-mer
    * @return The lexicographic index of the k-mer
    */
  private def kmerIndex (kmer: String, k: Int): Int = {
    kmer.map(c => if (charMap.contains(c)) charMap(c) else return -1)
        .reverse
        .zip(sigs)
        .foldLeft(0) { (acc, t) => acc + t._1 * t._2 }
  }

  /**
    * Write k-mer counts out to file
    * @param m List of k-mer counts
    * @param file_name Name of the file to write the k-mer counts to
    */
  private def writeCounts(m: List[CountRecord], file_name: String): Unit = {
    val pw = new PrintWriter(file_name)
    m.foreach { writeCount(_, pw) }
    pw.close()
  }

  private def writeCount(m: CountRecord, pw: PrintWriter): Unit = {
    pw.write(s">${m.id},${m.counts.mkString(",")}\n")
  }
}