using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;

namespace createEffect
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
        /// <summary>
        /// 生成效果配置文件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btn_Create_Click(object sender, EventArgs e)
        {
            txt_Preview.Clear();
            txt_Preview.ForeColor = System.Drawing.Color.Red;
            int result;
            foreach (Control c in this.Controls)//遍历窗体上所有控件Controls为所有窗体上控件的集合{if (c is TextBox)//判断某个控件是否TextBox
            {
                if (c is TextBox && c.Text.Trim() != "" && c.Name != "txt_Desc" && c.Name != "txt_shuoming")
                {
                    if (int.TryParse(c.Text, out result) == false)
                    {
                        MessageBox.Show("请输入数字 位置:" + c.Text);
                        return;
                    }
                    else
                    {
                        // MessageBox.Show(result.ToString());
                    }
                }
            }

            if (txt_ID.Text.Trim() == "" || txt_Desc.Text.Trim() == "")
            {
                txt_Preview.AppendText("缺少卡牌【ID】或者没有填写卡牌【名字】!");
                return;
            }
            if (txt_attackEndID.Text.Trim() != "" && txt_attackEndC.Text.Trim() == "")
            {
                txt_Preview.AppendText("【普通攻击结束】需要填写条件ID!");
                return;
            }
            if (txt_haloID.Text.Trim() != "" && (txt_haloCtype.Text.Trim() == "" || txt_haloC.Text.Trim() == ""))
            {
                txt_Preview.AppendText("【光环】需要填写条件类型和条件ID!");
                return;
            }
            if (txt_useattendID.Text.Trim() != "" && (txt_useattend1.Text.Trim() == "" || txt_useattend2.Text.Trim() == "" || txt_useattend3.Text.Trim() == ""))
            {
                txt_Preview.AppendText("【使用随从】需要填写条件的所有三个字段!");
                return;
            }
            if (txt_attendinID.Text.Trim() != "" && (txt_attendin1.Text.Trim() == "" || txt_attendin2.Text.Trim() == "" || txt_attendin3.Text.Trim() == ""))
            {
                txt_Preview.AppendText("【随从进场】需要填写条件的所有三个字段!");
                return;
            }
            if (txt_cureID.Text.Trim() != "" && (txt_cure1.Text.Trim() == "" || txt_cure2.Text.Trim() == "" || txt_cure3.Text.Trim() == ""))
            {
                txt_Preview.AppendText("【角色受治疗】需要填写条件的所有三个字段!");
                return;
            }
            if (txt_hurtID.Text.Trim() != "" && (txt_hurt1.Text.Trim() == "" || txt_hurt2.Text.Trim() == "" || txt_hurt3.Text.Trim() == ""))
            {
                txt_Preview.AppendText("【角色受伤】需要填写条件的所有三个字段!");
                return;
            }
            if (txt_deadID.Text.Trim() != "" && (txt_dead1.Text.Trim() == "" || txt_dead2.Text.Trim() == "" || txt_dead3.Text.Trim() == ""))
            {
                txt_Preview.AppendText("【角色死亡】需要填写条件的所有三个字段!");
                return;
            }
            txt_Preview.ForeColor = System.Drawing.Color.Black;
            txt_Preview.Clear();
            XmlDocument doc = new XmlDocument();
            doc.AppendChild(doc.CreateXmlDeclaration("1.0", "GB2312", null));

            XmlElement Root = doc.CreateElement("card");    //根节点
            doc.AppendChild(Root);
            XmlAttribute attr1 = doc.CreateAttribute("id");
            attr1.Value = txt_ID.Text.Trim();
            XmlAttribute attr2 = doc.CreateAttribute("name");
            attr2.Value = txt_Desc.Text.Trim();
            Root.Attributes.Append(attr1);
            Root.Attributes.Append(attr2);



            if (txt_beHurt.Text.Trim() != "")    //自身受伤时
            {
                XmlElement Child1 = doc.CreateElement("beHurtID");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("自身受伤时");
                Child1.AppendChild(comment);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_beHurt.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_beCure.Text.Trim() != "")    //自身受治疗时
            {
                XmlElement Child1 = doc.CreateElement("beCureID");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("自身受治疗时");
                Child1.AppendChild(comment);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_beCure.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_deadLanID.Text.Trim() != "")    //亡语
            {
                XmlElement Child1 = doc.CreateElement("deadLanguage");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("亡语");
                Child1.AppendChild(comment);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_deadLanID.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_sroundsID.Text.Trim() != "")    //自身回合开始时
            {
                XmlElement Child1 = doc.CreateElement("roundSID");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("自身回合开始时");
                Child1.AppendChild(comment);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_sroundsID.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_sroundeID.Text.Trim() != "")    //自身回合结束时
            {
                XmlElement Child1 = doc.CreateElement("roundEID");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("自身回合结束时");
                Child1.AppendChild(comment);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_sroundeID.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_eroundsID.Text.Trim() != "")    //敌方回合开始时
            {
                XmlElement Child1 = doc.CreateElement("enemyroundSID");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("敌方回合开始时");
                Child1.AppendChild(comment);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_eroundsID.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_eroundeID.Text.Trim() != "")    //敌方回合结束时
            {
                XmlElement Child1 = doc.CreateElement("enemyroundEID");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("敌方回合结束时");
                Child1.AppendChild(comment);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_eroundeID.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_selfUseMagic.Text.Trim() != "")    //己方使用法术牌时
            {
                XmlElement Child1 = doc.CreateElement("selfUseMagic");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("己方使用法术牌时");
                Child1.AppendChild(comment);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_selfUseMagic.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_enemyUseMagic.Text.Trim() != "")    //敌方使用法术牌时
            {
                XmlElement Child1 = doc.CreateElement("enemyUseMagic");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("敌方使用法术牌时");
                Child1.AppendChild(comment);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_enemyUseMagic.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_sAttack.Text.Trim() != "")    //自身开始攻击时
            {
                XmlElement Child1 = doc.CreateElement("sAttackID");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("自身开始攻击时");
                Child1.AppendChild(comment);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_sAttack.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_beAttack.Text.Trim() != "")    //自身被攻击时
            {
                XmlElement Child1 = doc.CreateElement("beAttackID");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("自身被攻击时");
                Child1.AppendChild(comment);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_beAttack.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_draw.Text.Trim() != "")    //自身抽牌时
            {
                XmlElement Child1 = doc.CreateElement("drawID");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("自身抽牌时");
                Child1.AppendChild(comment);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_draw.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_drawed.Text.Trim() != "")    //自身抽到该牌时
            {
                XmlElement Child1 = doc.CreateElement("drawedID");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("自身抽到该牌时");
                Child1.AppendChild(comment);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_drawed.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_hurtID.Text.Trim() != "")    //角色受伤时
            {
                XmlElement Child1 = doc.CreateElement("otherBeHurt");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("角色受伤时");
                Child1.AppendChild(comment);

                XmlElement conditionE = doc.CreateElement("condition");
                XmlAttribute attrID2 = doc.CreateAttribute("status");
                attrID2.Value = txt_hurt1.Text.Trim() + "-" + txt_hurt2.Text.Trim() + "-" + txt_hurt3.Text.Trim();
                conditionE.Attributes.Append(attrID2);
                Child1.AppendChild(conditionE);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_hurtID.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_cureID.Text.Trim() != "")    //角色受治疗时
            {
                XmlElement Child1 = doc.CreateElement("otherBeCure");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("角色受治疗时");
                Child1.AppendChild(comment);

                XmlElement conditionE = doc.CreateElement("condition");
                XmlAttribute attrID2 = doc.CreateAttribute("status");
                attrID2.Value = txt_cure1.Text.Trim() + "-" + txt_cure2.Text.Trim() + "-" + txt_cure3.Text.Trim();
                conditionE.Attributes.Append(attrID2);
                Child1.AppendChild(conditionE);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_cureID.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_deadID.Text.Trim() != "")    //角色死亡时
            {
                XmlElement Child1 = doc.CreateElement("otherDead");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("角色死亡时");
                Child1.AppendChild(comment);

                XmlElement conditionE = doc.CreateElement("condition");
                XmlAttribute attrID2 = doc.CreateAttribute("status");
                attrID2.Value = txt_dead1.Text.Trim() + "-" + txt_dead2.Text.Trim() + "-" + txt_dead3.Text.Trim();
                conditionE.Attributes.Append(attrID2);
                Child1.AppendChild(conditionE);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_deadID.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_useattendID.Text.Trim() != "")    //使用随从牌
            {
                XmlElement Child1 = doc.CreateElement("useAttend");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("使用随从牌");
                Child1.AppendChild(comment);

                XmlElement conditionE = doc.CreateElement("condition");
                XmlAttribute attrID2 = doc.CreateAttribute("status");
                attrID2.Value = txt_useattend1.Text.Trim() + "-" + txt_useattend2.Text.Trim() + "-" + txt_useattend3.Text.Trim();
                conditionE.Attributes.Append(attrID2);
                Child1.AppendChild(conditionE);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_useattendID.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_attendinID.Text.Trim() != "")    //随从进场
            {
                XmlElement Child1 = doc.CreateElement("attendIn");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("随从进场");
                Child1.AppendChild(comment);

                XmlElement conditionE = doc.CreateElement("condition");
                XmlAttribute attrID2 = doc.CreateAttribute("status");
                attrID2.Value = txt_attendin1.Text.Trim() + "-" + txt_attendin2.Text.Trim() + "-" + txt_attendin3.Text.Trim();
                conditionE.Attributes.Append(attrID2);
                Child1.AppendChild(conditionE);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_attendinID.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);
            }
            if (txt_attackEndID.Text.Trim() != "")   //普通攻击结束
            {

                XmlElement Child1 = doc.CreateElement("attackEnd");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("普通攻击结束");
                Child1.AppendChild(comment);

                XmlElement conditionE = doc.CreateElement("condition");
                XmlAttribute attrID2 = doc.CreateAttribute("id");
                attrID2.Value = txt_attackEndC.Text.Trim();
                conditionE.Attributes.Append(attrID2);
                Child1.AppendChild(conditionE);

                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_attackEndID.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);

            }
            if (txt_haloID.Text.Trim() != "")   //光环
            {

                XmlElement Child1 = doc.CreateElement("haloID");
                Root.AppendChild(Child1);
                XmlComment comment = doc.CreateComment("光环");
                Child1.AppendChild(comment);

                XmlElement conditionE = doc.CreateElement("haloCondition");
                XmlAttribute attrID3 = doc.CreateAttribute("ctype");
                attrID3.Value = txt_haloCtype.Text.Trim();
                conditionE.Attributes.Append(attrID3);
                Child1.AppendChild(conditionE);
                XmlAttribute attrID2 = doc.CreateAttribute("cid");
                attrID2.Value = txt_haloC.Text.Trim();
                conditionE.Attributes.Append(attrID2);


                XmlElement skillE = doc.CreateElement("skill");
                XmlAttribute attrID = doc.CreateAttribute("id");
                attrID.Value = txt_haloID.Text.Trim();
                skillE.Attributes.Append(attrID);
                Child1.AppendChild(skillE);

            }


            //这一行和上面顺序不能反//arr1就你的字段，如字段中有引号就要用\' ,最好不要用xml 的text段存内容
            //如果你有170 你的循环要对 应该有两个循环 一个在attr1 这 用于添加150个字段 一个在child1 用于添加几行

            // doc.InnerXml  这个属性就是你的xml 内容不带xml格式
            //txt_Preview.AppendText(doc.InnerXml);

            //string str_output = doc.InnerXml;
            doc.Save(txt_ID.Text + ".xml");//保存这个xml 网页或exe 都可以

            XmlDocument xmlDoc = new XmlDocument();//创建一个XML文档对象  
            xmlDoc.Load(txt_ID.Text + ".xml");//加载XML文档  
            StringWriter tw = new StringWriter();//定义一个StringWriter  
            XmlTextWriter tw2 = new XmlTextWriter(tw);//创建一个StringWriter实例的XmlTextWriter  
            tw2.Formatting = Formatting.Indented;//设置缩进  
            tw2.Indentation = 1;//设置缩进字数  
            tw2.IndentChar = '\t';//用\t字符作为缩进  
            xmlDoc.WriteTo(tw2);
            txt_Preview.Text += tw.ToString();
            tw.Close();
            
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            if(!File.Exists("说明.xml"))
            {
                txt_shuoming.Text = "你可以在这里添加说明";
                return;
            }
            XmlDocument doc = new XmlDocument();
            doc.Load("说明.xml");    //加载Xml文件  
            XmlElement rootElem = doc.DocumentElement;   //获取根节点  
            string str_shuoming = rootElem.InnerText;
            txt_shuoming.Text = str_shuoming;
        }

        private void btn_clear_Click(object sender, EventArgs e)
        {
            foreach (Control c in this.Controls)//遍历窗体上所有控件Controls为所有窗体上控件的集合{if (c is TextBox)//判断某个控件是否TextBox
            {
                if (c is TextBox && c.Name != "txt_shuoming")
                {
                    c.Text = "";
                }
            }
        }

        private void btn_save_Click(object sender, EventArgs e)
        {
            XmlDocument doc = new XmlDocument();
            doc.AppendChild(doc.CreateXmlDeclaration("1.0", "GB2312", null));

            XmlElement Root = doc.CreateElement("shuoming");    //根节点
            doc.AppendChild(Root);
            Root.InnerText = txt_shuoming.Text;
            doc.Save("说明" + ".xml");//保存这个xml 网页或exe 都可以

            txt_shuoming.ReadOnly = true;
        }

        private void btn_edit_Click(object sender, EventArgs e)
        {
            txt_shuoming.ReadOnly = false;
        }
    }
}
