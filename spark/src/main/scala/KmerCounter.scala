
import scala.io.Source

object KmerCounter {

  def main(args: Array[String]): Unit = {
    val fasta_file = "/Users/jonpdeaton/Datasets/viruses/all_phage_genomes.fasta"
    val p = FastaParser(fasta_file)
  }
}