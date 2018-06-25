
import scala.io.Source
import FastaParser

object KmerCounter {

  def main(args: Array[String]) {

    val fasta_file = "/Users/jonpdeaton/Datasets/viruses/all_phage_genomes.fasta"

    val p = FastaParser(fasta_file)

  }
}