import scala.util.parsing.combinator._

class FastaParser {

  case class Entry(description: String, sequence: String)

  def fromFile(fn: String): List[Entry] = {
    val lines = io.Source.fromFile(fn).getLines.mkString("\n")
    fromString(lines)
  }

  def fromString(input: String): List[Entry] =
    Parser.parse(input)

  private object Parser extends RegexParsers {
    lazy val header: Parser[String] = """>.*""".r ^^ { _.tail.trim }
    lazy val seqLine: Parser[String] = """[^>].*""".r ^^ { _.trim }
    lazy val sequence: Parser[String] = rep1(seqLine) ^^ { _.mkString }

    lazy val entry: Parser[Entry] = header ~ sequence ^^ {
      case h ~ s => Entry(h,s)
    }

    lazy val entries: Parser[List[Entry]] = rep1(entry)

    def parse(input: String): List[Entry]  = {
      parseAll(entries, input) match {
        case Success(es , _) => es
        case x: NoSuccess =>  throw new Exception(x.toString)
      }
    }
  }
}
