// 入力例1(0_sample_00.in)  : OK
// 入力例2(0_sample_01.in)  : OK
// 出力例1(0_sample_00.diff): NG
// diff tmp/wiki_1.in ../../A_Sendame/tests/0_sample_00.in
// + hoge
// - hoge
import java.io.{File,PrintWriter}
import scala.sys.process.{Process,ProcessLogger}
object sample_diff {
  def sample_?(fname: String) =
    fname.contains("sample") && (fname.endsWith(".in") || fname.endsWith(".diff"))
  def sample_in_?(fname: String) =
    sample_?(fname) && fname.endsWith(".in")
  def sample_diff_?(fname: String) =
    sample_?(fname) && fname.endsWith(".diff")
  def diff(wiki: (String, String), test: String) {
    val comseq = Seq("diff", "--strip-trailing-cr", wiki._2, test)
    val pb = Process(comseq)
    if (pb ! ProcessLogger(str => ()) == 0)
      println("%-30s".format("%s(%s)".format(wiki._1, test)) + ": OK")
    else {
      println("%-30s".format("%s(%s)".format(wiki._1, test)) + ": NG")
      println(comseq.mkString(" "))
      pb.lineStream_! foreach println
    }
  }
  def examples(wiki_page: File, tmp_dir: File, sec: String, post: String): Seq[(String, String)] = {
    val sc = new java.util.Scanner(wiki_page, "UTF-8")
    val res = (Nil : Seq[(String, String)]).toBuffer
    var exname: String = null
    var fname : String = null
    var pw: PrintWriter = null
    while (sc.hasNextLine) {
      val s  = sc.nextLine
      if (s startsWith "*** " + sec) {
        var end = s.indexOf("[")
        if (end == -1) {
          end = s.length
        }
        exname = s.substring(s.lastIndexOf("*")+1, end).trim
        fname  = tmp_dir.toString + "/wiki_" + exname.last + post
        pw     = new PrintWriter(fname)
      }
      else if (pw != null) {
        if (s startsWith " ")
          pw.print(s.tail + "\n")
        else {
          res += ((exname, fname))
          pw.flush; pw.close
          pw = null
        }
      }
    }
    res.toSeq
  }
  def main(args: Array[String]) {
    val wiki_page = new File(args(0))
    val tests_dir = new File(args(1))
    val tmp_dir   = new File(args(2)); tmp_dir.mkdirs

    val wiki_in    = examples(wiki_page, tmp_dir, "入力例", ".in")  // (入力例*, wiki_*.in)
    val wiki_diff  = examples(wiki_page, tmp_dir, "出力例", ".diff") // (出力例*, wiki_*.out)

    val tests_in   = tests_dir.listFiles.filter(e => sample_in_?(e.getName)).map(_.toString)     // (*sample_*.in)
    val tests_diff = tests_dir.listFiles.filter(e => sample_diff_?(e.getName)).map(_.toString)   // (*sample_*.diff)

    val tests_in_s = tests_in.sorted
    val tests_diff_s = tests_diff.sorted

    if (wiki_in.size != tests_in_s.size)
      println("the number of inputs is different")
    wiki_in.zip(tests_in_s) foreach { e =>
      diff(e._1, e._2)
    }

    if (wiki_diff.size != tests_diff_s.size)
      println("[WARNING] the number of outputs is different")
    wiki_diff.zip(tests_diff_s) foreach { e =>
      diff(e._1, e._2)
    }
  }
}
