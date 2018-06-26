package kmercounter

import scala.util.parsing.combinator._

case class FastaRecord(id: String, sequence: String)

class FastaParser {

  def fromFile(file_name: String): List[FastaRecord] = {
    val lines = io.Source.fromFile(file_name).getLines.mkString("\n")
    fromString(lines)
  }

  def fromString(input: String): List[FastaRecord] =
    Parser.parse(input)

  private object Parser extends RegexParsers {
    lazy val header: Parser[String] = """>.*""".r ^^ { _.tail.trim }
    lazy val seqLine: Parser[String] = """[^>].*""".r ^^ { _.trim }
    lazy val sequence: Parser[String] = rep1(seqLine) ^^ { _.mkString }

    lazy val entry: Parser[FastaRecord] = header ~ sequence ^^ {
      case h ~ s => FastaRecord(h, s.toUpperCase)
    }

    lazy val entries: Parser[List[FastaRecord]] = rep1(entry)

    def parse(input: String): List[FastaRecord] = {
      parseAll(entries, input) match {
        case Success(es , _) => es
        case x: NoSuccess =>  throw new Exception(x.toString)
      }
    }
  }
}
