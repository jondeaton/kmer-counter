package kmercounter

import java.io.File

case class Config(k: Int = 4, out: File = new File("."), in: File = new File("."),
                  spark: Boolean = false,
                  verbose: Boolean = false, debug: Boolean = false)

object KmerCounterMain {

  def main(args: Array[String]): Unit = {

    val parser = new scopt.OptionParser[Config]("kmer-counter") {
      head("k-mer counter", "1.0")

      opt[Int]('k', "k").action( (x, c) =>
        c.copy(k = x) ).text("Length of k-mers to count")

      opt[File]('i', "in").required().valueName("<file>").
        action( (x, c) => c.copy(in = x) ).
        text("input FASTA file (required)")

      opt[File]('o', "out").required().valueName("<file>").
        action( (x, c) => c.copy(out = x) ).
        text("output k-mer count file (required)")

      opt[Unit]("spark").action( (_, c) =>
        c.copy(spark = true) ).text("run on Apache Spark")

      opt[Unit]("verbose").action( (_, c) =>
        c.copy(verbose = true) ).text("verbose is a flag")

      opt[Unit]("debug").hidden().action( (_, c) =>
        c.copy(debug = true) ).text("this option is hidden in the usage text")

      help("help").text("prints this usage text")
    }

    // parser.parse returns Option[C]
    parser.parse(args, Config()) match {
      case Some(config) =>
        val kmer_counter = new KmerCounter
        if (config.verbose)
          println(s"Counting file: ${config.in}")
        kmer_counter.countFile(config.in, config.out, config.k)

      case None =>
        // arguments are bad, error message will have been displayed
    }

  }
}
