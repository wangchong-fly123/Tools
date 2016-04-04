using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace SkillParser
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        enum TargetType
        {
            ATTACK_TARGET_NONE = 0,
            ATTACK_TARGET_EHERO = 1,	//敌方英雄
            ATTACK_TARGET_EATTEND = 2,	//敌方随从
            ATTACK_TARGET_SHERO = 4,	//己方英雄
            ATTACK_TARGET_SATTEND = 8,	//己方随从
            ATTACK_TARGET_EEQUIP = 16,     //敌方武器
            ATTACK_TARGET_SEQUIP = 32,     //己方武器
            ATTACK_TARGET_MYSELF = 64,     //施法者自身
            ATTACK_TARGET_LEFT_RIGHT = 128,    //两侧
            ATTACK_TARGET_SHAND = 256,    //己方手牌
            ATTACK_TARGET_EHAND = 512,    //敌方手牌
        }

        public void parserOneEffect(string effect)
        {
            string[] one = effect.Split(':');
            if (one.Length != 2)
            {
                txt_output.Text = "效果配置错误 “：”符号前后有问题！！！";
                return;
            }
            String effectHead = one[0];
            String effectBody = one[1];
            if (effectHead.Length == 0)
            {
                txt_output.Text = "效果配置错误 “：”符号前有问题！！！";
                return;
            }
            if (effectBody.Length == 0)
            {
                txt_output.Text = "效果配置错误 “：”符号后有问题！！！";
                return;
            }
            string[] head = effectHead.Split('-');
            if (head.Length != 7)
            {
                txt_output.Text = "效果配置错误 “：”符号前有问题！！！目标段部分配置出错";
                return;
            }
            Int32 target = Convert.ToInt32(head[3].ToString());
            Int32 targetCount = 0;
            String name = "(";
            int flag = 0;
            flag = target & (Int32)(TargetType.ATTACK_TARGET_EHERO);
            if (flag > 0)
            {
                name += "敌方英雄,";
                targetCount++;
            }
            flag = target & (Int32)(TargetType.ATTACK_TARGET_EATTEND);
            if (flag > 0)
            {
                name += "敌方随从,";
                targetCount++;
            }
            flag = target & (Int32)(TargetType.ATTACK_TARGET_SHERO);
            if (flag > 0)
            {
                name += "己方英雄,";
                targetCount++;
            }
            flag = target & (Int32)(TargetType.ATTACK_TARGET_SATTEND);
            if (flag > 0)
            {
                name += "己方随从,";
                targetCount++;
            }
            flag = target & (Int32)(TargetType.ATTACK_TARGET_MYSELF);
            if (flag > 0)
            {
                name += "自身";
                targetCount++;
            }
            //flag = target & (Int32)(TargetType.ATTACK_TARGET_LEFT_RIGHT);
            //if (flag > 0)
            //{
            //    name += "两侧";
            //    targetCount++;
            //}
            flag = target & (Int32)(TargetType.ATTACK_TARGET_SHAND);
            if (flag > 0)
            {
                name += "己方手牌,";
                targetCount++;
            }
            flag = target & (Int32)(TargetType.ATTACK_TARGET_EHAND);
            if (flag > 0)
            {
                name += "敌方手牌,";
                targetCount++;
            }
            flag = target & (Int32)(TargetType.ATTACK_TARGET_SEQUIP);
            if (flag > 0)
            {
                name += "己方武器,";
                targetCount++;
            }
            flag = target & (Int32)(TargetType.ATTACK_TARGET_EEQUIP);
            if (flag > 0)
            {
                name += "敌方武器,";
                targetCount++;
            }
            Int32 include = Convert.ToInt32(head[4].ToString()); //包括施法者
            if (include == 0)
            {
                name += ":排除施法者";
            }
            Int32 useHand = Convert.ToInt32(head[6].ToString()); //使用手选的


            Int32 flag_left_right = target & (Int32)(TargetType.ATTACK_TARGET_LEFT_RIGHT);
            
            switch (Convert.ToInt32(head[1].ToString()))
            {
                case 1:
                    txt_output.AppendText("单体");
                    if (targetCount > 1)
                    {
                        txt_output.AppendText("对" + name + ")中其中一个单位:");
                    }
                    else
                    {
                        txt_output.AppendText("对" + name + "):");
                    }
                    
                    if (useHand == 1)
                    {
                        name += "<使用手选的目标>";
                    }

                    if (flag_left_right > 0)
                    {
                        txt_output.AppendText("的两侧");
                    }
                    break;
                case 2:
                    txt_output.AppendText("群体");
                    txt_output.AppendText("对" + name + ")所有单位:");
                    break;
                case 3:
                    txt_output.AppendText("随机目标(次数)");
                    txt_output.AppendText("在" + name + ")中随机" + head[2].ToString() + "次,对每次随机到的单位:");
                    if (flag_left_right > 0)
                    {
                        txt_output.AppendText("的两侧");
                    }
                    break;
                case 4:
                    txt_output.AppendText("随机目标(个数)");
                    txt_output.AppendText("在" + name + ")中随机" + head[2].ToString() + "个,对每个随机到的单位:");
                    if (flag_left_right > 0)
                    {
                        txt_output.AppendText("的两侧");
                    }
                    break;
                default:
                    break;
            }


            string[] bodys = effectBody.Split(';');
            foreach (string i in bodys)
            {
                if (i.Length > 0)
                {
                    parserOneEffectBody(i);
                    txt_output.AppendText(" ");
                }
            }
        }

        public void parserOneEffectBody(string body)
        {
            string[] content = body.Split('-');
            if (content.Length != 6)
            {
                txt_output.Text = "效果配置错误 “：”符号后有问题！！！效果体配置出错";
                return;
            }
            Int32 status = Convert.ToInt32(content[0].ToString());
            //if (status >= 1000)
            //{
            //    if (status >= 1000 && status <= 1999)
            //    {
            //        txt_output.AppendText("增加 +" + content[2].ToString() + "血");
            //    }
            //    else if (status >= 2000 && status <= 2999)
            //    {
            //        txt_output.AppendText("本回合增加 +" + content[2].ToString() + "血");
            //    }
            //    else if (status >= 3000 && status <= 3999)
            //    {
            //        txt_output.AppendText("增加 +" + content[2].ToString() + "攻击");
            //    }
            //    else if (status >= 4000 && status <= 4999)
            //    {
            //        txt_output.AppendText("本回合增加 +" + content[2].ToString() + "攻击");
            //    }
            //}
            //else
            {
                switch (status)
                {
                    case 1:
                        txt_output.AppendText("造成" + content[2].ToString() + "点伤害");
                        break;
                    case 2:
                        {
                            Int32 hp = Convert.ToInt32(content[2].ToString());
                            if (hp == 0)
                            {
                                txt_output.AppendText("恢复全部生命值");
                            }
                            else
                            {
                                txt_output.AppendText("恢复" + hp + "点生命");
                            }
                        }
                        break;
                    case 3:
                        txt_output.AppendText("抽取" + content[2].ToString() + "张牌");
                        break;
                    case 4:
                        txt_output.AppendText("进行消灭");
                        break;
                    case 5:
                        txt_output.AppendText("增加" + content[2].ToString() + "法力水晶");
                        break;
                    case 6:
                        txt_output.AppendText("增加" + content[2].ToString() + "法力水晶上限");
                        break;
                    case 7:
                        txt_output.AppendText("召唤卡牌ID为" + content[2].ToString() + "的随从" + content[3].ToString() + "个");
                        break;
                    case 8:
                        txt_output.AppendText("添加圣盾");
                        break;
                    case 9:
                        txt_output.AppendText("添加冲锋");
                        break;
                    case 10:
                        txt_output.AppendText("添加嘲讽");
                        break;
                    case 11:
                        txt_output.AppendText("添加风怒");
                        break;
                    case 12:
                        txt_output.AppendText("添加潜行");
                        break;
                    case 13:
                        txt_output.AppendText("随机弃" + content[2].ToString() + "张手牌");
                        break;
                    case 14:
                        txt_output.AppendText("增加" + content[2].ToString() + "护甲");
                        break;
                    case 15:
                        txt_output.AppendText("设置攻击力为" + content[2].ToString());
                        break;
                    case 16:
                        txt_output.AppendText("设置血量为" + content[2].ToString());
                        break;
                    case 17:
                        txt_output.AppendText("添加亡语ID:" + content[2].ToString());
                        break;
                    case 18:
                        txt_output.AppendText("添加自身回合开始ID:" + content[2].ToString());
                        break;
                    case 19:
                        txt_output.AppendText("添加自身回合结束ID:" + content[2].ToString());
                        break;
                    case 20:
                        txt_output.AppendText("减少攻击力:" + content[2].ToString());
                        break;
                    case 21:
                        txt_output.AppendText("造成:" + content[2].ToString() + "到" + content[3].ToString() + "点伤害");
                        break;
                    case 22:
                        txt_output.AppendText("造成:" + content[2].ToString() + "到" + content[3].ToString() + "点伤害（受法伤加成有效）");
                        break;
                    case 23:
                        txt_output.AppendText("清除潜行");
                        break;
                    case 24:
                        txt_output.AppendText("造成等同于目标攻击力的伤害");
                        break;
                    case 25:
                        txt_output.AppendText("血量乘以:" + content[2].ToString());
                        break;
                    case 26:
                        txt_output.AppendText("攻击力乘以:" + content[2].ToString());
                        break;
                    case 27:
                        txt_output.AppendText("回到拥有者手牌");
                        break;
                    case 28:
                        txt_output.AppendText("回到己方手牌");
                        break;
                    case 29:
                        txt_output.AppendText("回到敌方手牌");
                        break;
                    case 30:
                        txt_output.AppendText("获得:法术伤害增加+" + content[2].ToString());
                        break;
                    case 31:
                        txt_output.AppendText("从" + content[2].ToString() + "库召唤" + content[3].ToString() + "个随从到战场");
                        break;
                    case 32:
                        txt_output.AppendText("从" + content[2].ToString() + "库召唤" + content[3].ToString() + "个卡牌到手牌");
                        break;
                    case 33:
                        txt_output.AppendText("添加敌方回合开始ID:" + content[2].ToString());
                        break;
                    case 34:
                        txt_output.AppendText("添加敌方回合结束ID:" + content[2].ToString());
                        break;
                    case 35:
                        txt_output.AppendText("复制" + content[2].ToString() + "张目标到己方手牌");
                        break;
                    case 36:
                        txt_output.AppendText("复制" + content[2].ToString() + "张目标到敌方手牌");
                        break;
                    case 37:
                        txt_output.AppendText("复制" + content[2].ToString() + "张目标到己方牌库");
                        break;
                    case 38:
                        txt_output.AppendText("复制" + content[2].ToString() + "张目标到敌方牌库");
                        break;
                    case 39:
                        txt_output.AppendText("洗回目标到己方牌库");
                        break;
                    case 40:
                        txt_output.AppendText("洗回目标到敌方牌库");
                        break;
                    case 41:
                        txt_output.AppendText("与目标交换血量");
                        break;
                    case 42:
                        txt_output.AppendText("目标随从攻血互换");
                        break;
                    case 43:
                        txt_output.AppendText("将目标变为" + content[2].ToString());
                        break;
                    case 44:
                        txt_output.AppendText("造成等同攻击者攻击力的伤害");
                        break;
                    case 45:
                        txt_output.AppendText("将目标变为" + content[2].ToString() + "或" + content[3].ToString());
                        break;
                    case 46:
                        txt_output.AppendText("法力消耗增加" + content[2].ToString());
                        break;
                    case 47:
                        txt_output.AppendText("法力消耗减少" + content[2].ToString());
                        break;
                    case 48:
                        txt_output.AppendText("复制目标到己方战场");
                        break;
                    case 49:
                        txt_output.AppendText("复制目标到敌方战场");
                        break;
                    case 1000:
                        txt_output.AppendText("获得:+" + content[2].ToString() + "/+" + content[3].ToString());
                        break;
                    default:
                        break;
                }
            }
        }

        private void btn_translate_Click(object sender, EventArgs e)
        {
            String str_input = "";
            str_input = txt_input.Text.Trim();
            txt_output.Clear();
            if (str_input.Length == 0)
            {
                txt_output.Text = "输入不能为空";
                return;
            }
            List<String> effectList = new List<string>();
            string[] effect = str_input.Split('.');
            foreach (string i in effect)
            {
                if (i.Length > 0)
                {
                    parserOneEffect(i.ToString());
                    txt_output.AppendText("。 ");
                    txt_output.AppendText("\r\n");
                }
            }

        }

        private void btn_clearInput_Click(object sender, EventArgs e)
        {
            txt_input.Clear();
        }

        private void radioButton4_CheckedChanged(object sender, EventArgs e)
        {
            txt_random.Enabled = true;
            chk_left_right.Enabled = true;
            chk_myself.Enabled = false;
        }

        private void btn_create_Click(object sender, EventArgs e)
        {
            txt_outstr.Clear();
            txt_output.Clear();
            

            if(radio_3.Checked || radio_4.Checked)
            {
                if(txt_random.Text.Trim() == "")
                {
                    txt_outstr.Text = "【随机数值】必须填写";
                    return;
                }
                int result;
                if (int.TryParse(txt_random.Text, out result) == false)
                {
                    MessageBox.Show("请输入数字 位置:" + txt_random.Text);
                    return;
                }
            }
            if(txt_condition.Text.Trim() != "")
            {
                int result;
                if (int.TryParse(txt_condition.Text, out result) == false)
                {
                    MessageBox.Show("请输入数字 位置:" + txt_condition.Text);
                    return;
                }
            }
            if (txt_percent.Text.Trim() != "")
            {
                int result;
                if (int.TryParse(txt_percent.Text, out result) == false)
                {
                    MessageBox.Show("请输入数字 位置:" + txt_percent.Text);
                    return;
                }
            }
            if (txt_value1.Text.Trim() != "")
            {
                int result;
                if (int.TryParse(txt_value1.Text, out result) == false)
                {
                    MessageBox.Show("请输入数字 位置:" + txt_value1.Text);
                    return;
                }
            }
            if (txt_value2.Text.Trim() != "")
            {
                int result;
                if (int.TryParse(txt_value2.Text, out result) == false)
                {
                    MessageBox.Show("请输入数字 位置:" + txt_value2.Text);
                    return;
                }
            }
            if (txt_state.Text.Trim() != "")
            {
                int result;
                if (int.TryParse(txt_state.Text, out result) == false)
                {
                    MessageBox.Show("请输入数字 位置:" + txt_state.Text);
                    return;
                }
            }
            string str_output = "0-";
            if(radio_1.Checked)
            {
                str_output += "1-";
            }
            else if (radio_2.Checked)
            {
                str_output += "2-";
            }
            else if (radio_3.Checked)
            {
                str_output += "3-";
            }
            else if (radio_4.Checked)
            {
                str_output += "4-";
            }
            if (radio_3.Checked || radio_4.Checked)
            {
                if (txt_random.Text.Trim() != "")
                {
                    str_output += txt_random.Text.Trim()+"-";
                }
            }
            else
            {
                str_output += "0-";
            }

            Int32 target = 0;
            /////////////////////作用范围//////////
            if(chk_ehero.Checked)
            {
                target += Convert.ToInt32(TargetType.ATTACK_TARGET_EHERO);
            }
            if(chk_shero.Checked)
            {
                target += Convert.ToInt32(TargetType.ATTACK_TARGET_SHERO);
            }
            if(chk_eequip.Checked)
            {
                target += Convert.ToInt32(TargetType.ATTACK_TARGET_EEQUIP);
            }
            if(chk_sequip.Checked)
            {
                target += Convert.ToInt32(TargetType.ATTACK_TARGET_SEQUIP);
            }
            if(chk_eattend.Checked)
            {
                target += Convert.ToInt32(TargetType.ATTACK_TARGET_EATTEND);
            }
            if(chk_sattend.Checked)
            {
                target += Convert.ToInt32(TargetType.ATTACK_TARGET_SATTEND);
            }
            if(chk_ehand.Checked)
            {
                target += Convert.ToInt32(TargetType.ATTACK_TARGET_EHAND);
            }
            if(chk_shand.Checked)
            {
                target += Convert.ToInt32(TargetType.ATTACK_TARGET_SHAND);
            }
            if(chk_myself.Checked)
            {
                target += Convert.ToInt32(TargetType.ATTACK_TARGET_MYSELF);
            }
            if(chk_left_right.Checked)
            {
                target += Convert.ToInt32(TargetType.ATTACK_TARGET_LEFT_RIGHT);
            }
            if(target > 0)
            {
                str_output += target.ToString()+"-";
            }
            else
            {
                txt_outstr.Text = "目标范围必须选";
                return;
            }
            
            if(chk_include.Checked)
            {
                str_output += "1-";
            }
            else
            {
                str_output += "0-";
            }

            if(txt_condition.Text.Trim() != "")
            {
                str_output += txt_condition.Text.Trim()+"-";
            }
            else
            {
                str_output += "0-";
            }
            if(chk_useHand.Checked)
            {
                str_output += "1";
            }
            else
            {
                str_output += "0";
            }
            str_output += ":";
           // if(comb_effect.SelectedIndex > 0)
           // {
                str_output += (comb_effect.SelectedIndex + 1).ToString() + "-" + txt_percent.Text.Trim() + "-";
          //  }
           // else
           // {
           //     return;
          //  }
          
            if (txt_value1.Text.Trim() !="")
            {
                str_output += txt_value1.Text.Trim() + "-";
            }
            else
            {
                str_output += "0-";
            }
            if (txt_value2.Text.Trim() != "")
            {
                str_output += txt_value2.Text.Trim() + "-";
            }
            else
            {
                str_output += "0-";
            }
            if(chk_IsHalo.Checked)
            {
                str_output += "1-";
            }
            else
            {
                str_output += "0-";
            }
            if(txt_state.Text.Trim() !="")
            {
                str_output += txt_state.Text.Trim();
            }
            else
            {
                str_output += "0";
            }
            str_output += ";.";
            txt_outstr.Text = str_output;

            //////////////////////////////////////////////////////////////////////////////////////
            String str_input = "";
            str_input = str_output;
            
            if (str_input.Length == 0)
            {
                //txt_output.Text = "输入不能为空";
                return;
            }
            List<String> effectList = new List<string>();
            string[] effect = str_input.Split('.');
            foreach (string i in effect)
            {
                if (i.Length > 0)
                {
                    parserOneEffect(i.ToString());
                    
                    
                }
            }
        }

        private void radio_3_CheckedChanged(object sender, EventArgs e)
        {
            txt_random.Enabled = true;
            chk_left_right.Enabled = true;
            chk_myself.Enabled = false;
        }

        private void radio_2_CheckedChanged(object sender, EventArgs e)
        {
            txt_random.Enabled = false;
            chk_left_right.Enabled = false;
            chk_myself.Enabled = false;
        }

        private void radio_1_CheckedChanged(object sender, EventArgs e)
        {
            txt_random.Enabled = false;
            chk_left_right.Enabled = true;
            chk_myself.Enabled = true;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            radio_1.Checked = true;
            txt_percent.Text = "100";
            comb_effect.SelectedIndex = 0;
        }

        private void btn_reset_Click(object sender, EventArgs e)
        {
            foreach (Control c in this.Controls)//遍历窗体上所有控件Controls为所有窗体上控件的集合{if (c is TextBox)//判断某个控件是否TextBox
            {
                if (c is CheckBox)
                {
                    ((CheckBox)c).Checked = false;
                }
            }
            txt_random.Clear();
            txt_value1.Clear();
            txt_value2.Clear();
            txt_state.Clear();
            txt_condition.Clear();

        }

        private void chk_left_right_CheckedChanged(object sender, EventArgs e)
        {
            chk_ehero.Checked = false;
            chk_shero.Checked = false;
            chk_eequip.Checked = false;
            chk_sequip.Checked = false;
            chk_ehand.Checked = false;
            chk_shand.Checked = false;
        }
    }
}
