#CSharp.load("System.Windows.Forms.Form", "System.Windows.Forms, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089");
#form = CSharp::Form.new
#
#CSharp.load("MRubySample.ShowWindow", null);
#CSharp::ShowWindow.Show("Hello!!")

#class Test
#  def test
#    puts 'Hello!'
#  end
#end
#
#test = Test.new
#test.test

Kernel.methods.each do |m|
  puts m
end

