
CSharp.load("System.Windows.Forms, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089");

#CSharp.using("System.Windows.Forms.MessageBox");
#CSharp::MessageBox.Show("C# に mruby を組み込んでみた");
#CSharp::MessageBox.Show("悪魔合体mrubysharp");


CSharp.using("System.Windows.Forms.Form");
form = CSharp::Form.new
form.ShowDialog();


