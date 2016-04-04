namespace SkillParser
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.txt_input = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.btn_translate = new System.Windows.Forms.Button();
            this.txt_output = new System.Windows.Forms.TextBox();
            this.btn_clearInput = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.radio_4 = new System.Windows.Forms.RadioButton();
            this.radio_3 = new System.Windows.Forms.RadioButton();
            this.radio_2 = new System.Windows.Forms.RadioButton();
            this.radio_1 = new System.Windows.Forms.RadioButton();
            this.chk_ehero = new System.Windows.Forms.CheckBox();
            this.chk_shero = new System.Windows.Forms.CheckBox();
            this.chk_eattend = new System.Windows.Forms.CheckBox();
            this.chk_sattend = new System.Windows.Forms.CheckBox();
            this.chk_eequip = new System.Windows.Forms.CheckBox();
            this.chk_sequip = new System.Windows.Forms.CheckBox();
            this.chk_ehand = new System.Windows.Forms.CheckBox();
            this.chk_shand = new System.Windows.Forms.CheckBox();
            this.txt_random = new System.Windows.Forms.TextBox();
            this.chk_include = new System.Windows.Forms.CheckBox();
            this.txt_condition = new System.Windows.Forms.TextBox();
            this.chk_useHand = new System.Windows.Forms.CheckBox();
            this.txt_percent = new System.Windows.Forms.TextBox();
            this.chk_IsHalo = new System.Windows.Forms.CheckBox();
            this.txt_value1 = new System.Windows.Forms.TextBox();
            this.txt_value2 = new System.Windows.Forms.TextBox();
            this.txt_state = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.btn_create = new System.Windows.Forms.Button();
            this.txt_outstr = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.chk_myself = new System.Windows.Forms.CheckBox();
            this.chk_left_right = new System.Windows.Forms.CheckBox();
            this.comb_effect = new System.Windows.Forms.ComboBox();
            this.btn_reset = new System.Windows.Forms.Button();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // txt_input
            // 
            this.txt_input.Font = new System.Drawing.Font("SimSun", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.txt_input.Location = new System.Drawing.Point(48, 53);
            this.txt_input.Multiline = true;
            this.txt_input.Name = "txt_input";
            this.txt_input.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txt_input.Size = new System.Drawing.Size(544, 119);
            this.txt_input.TabIndex = 23;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(46, 38);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(77, 12);
            this.label1.TabIndex = 1;
            this.label1.Text = "输入技能效果";
            // 
            // btn_translate
            // 
            this.btn_translate.Location = new System.Drawing.Point(48, 185);
            this.btn_translate.Name = "btn_translate";
            this.btn_translate.Size = new System.Drawing.Size(111, 31);
            this.btn_translate.TabIndex = 25;
            this.btn_translate.Text = "翻译效果";
            this.btn_translate.UseVisualStyleBackColor = true;
            this.btn_translate.Click += new System.EventHandler(this.btn_translate_Click);
            // 
            // txt_output
            // 
            this.txt_output.Font = new System.Drawing.Font("SimSun", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.txt_output.Location = new System.Drawing.Point(48, 230);
            this.txt_output.Multiline = true;
            this.txt_output.Name = "txt_output";
            this.txt_output.ReadOnly = true;
            this.txt_output.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txt_output.Size = new System.Drawing.Size(544, 224);
            this.txt_output.TabIndex = 24;
            // 
            // btn_clearInput
            // 
            this.btn_clearInput.Location = new System.Drawing.Point(598, 53);
            this.btn_clearInput.Name = "btn_clearInput";
            this.btn_clearInput.Size = new System.Drawing.Size(68, 119);
            this.btn_clearInput.TabIndex = 26;
            this.btn_clearInput.Text = "<<清空";
            this.btn_clearInput.UseVisualStyleBackColor = true;
            this.btn_clearInput.Click += new System.EventHandler(this.btn_clearInput_Click);
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.textBox1);
            this.panel1.Controls.Add(this.radio_4);
            this.panel1.Controls.Add(this.radio_3);
            this.panel1.Controls.Add(this.radio_2);
            this.panel1.Controls.Add(this.radio_1);
            this.panel1.Location = new System.Drawing.Point(694, 85);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(141, 100);
            this.panel1.TabIndex = 5;
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(153, 33);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(54, 21);
            this.textBox1.TabIndex = 12;
            // 
            // radio_4
            // 
            this.radio_4.AutoSize = true;
            this.radio_4.Location = new System.Drawing.Point(14, 67);
            this.radio_4.Name = "radio_4";
            this.radio_4.Size = new System.Drawing.Size(83, 16);
            this.radio_4.TabIndex = 3;
            this.radio_4.TabStop = true;
            this.radio_4.Text = "随机（个）";
            this.radio_4.UseVisualStyleBackColor = true;
            this.radio_4.CheckedChanged += new System.EventHandler(this.radioButton4_CheckedChanged);
            // 
            // radio_3
            // 
            this.radio_3.AutoSize = true;
            this.radio_3.Location = new System.Drawing.Point(14, 46);
            this.radio_3.Name = "radio_3";
            this.radio_3.Size = new System.Drawing.Size(83, 16);
            this.radio_3.TabIndex = 2;
            this.radio_3.TabStop = true;
            this.radio_3.Text = "随机（次）";
            this.radio_3.UseVisualStyleBackColor = true;
            this.radio_3.CheckedChanged += new System.EventHandler(this.radio_3_CheckedChanged);
            // 
            // radio_2
            // 
            this.radio_2.AutoSize = true;
            this.radio_2.Location = new System.Drawing.Point(14, 25);
            this.radio_2.Name = "radio_2";
            this.radio_2.Size = new System.Drawing.Size(47, 16);
            this.radio_2.TabIndex = 1;
            this.radio_2.TabStop = true;
            this.radio_2.Text = "群体";
            this.radio_2.UseVisualStyleBackColor = true;
            this.radio_2.CheckedChanged += new System.EventHandler(this.radio_2_CheckedChanged);
            // 
            // radio_1
            // 
            this.radio_1.AutoSize = true;
            this.radio_1.Location = new System.Drawing.Point(14, 4);
            this.radio_1.Name = "radio_1";
            this.radio_1.Size = new System.Drawing.Size(47, 16);
            this.radio_1.TabIndex = 0;
            this.radio_1.TabStop = true;
            this.radio_1.Text = "单体";
            this.radio_1.UseVisualStyleBackColor = true;
            this.radio_1.CheckedChanged += new System.EventHandler(this.radio_1_CheckedChanged);
            // 
            // chk_ehero
            // 
            this.chk_ehero.AutoSize = true;
            this.chk_ehero.Location = new System.Drawing.Point(977, 30);
            this.chk_ehero.Name = "chk_ehero";
            this.chk_ehero.Size = new System.Drawing.Size(72, 16);
            this.chk_ehero.TabIndex = 1;
            this.chk_ehero.Text = "敌方英雄";
            this.chk_ehero.UseVisualStyleBackColor = true;
            // 
            // chk_shero
            // 
            this.chk_shero.AutoSize = true;
            this.chk_shero.Location = new System.Drawing.Point(977, 51);
            this.chk_shero.Name = "chk_shero";
            this.chk_shero.Size = new System.Drawing.Size(72, 16);
            this.chk_shero.TabIndex = 2;
            this.chk_shero.Text = "己方英雄";
            this.chk_shero.UseVisualStyleBackColor = true;
            // 
            // chk_eattend
            // 
            this.chk_eattend.AutoSize = true;
            this.chk_eattend.Location = new System.Drawing.Point(977, 72);
            this.chk_eattend.Name = "chk_eattend";
            this.chk_eattend.Size = new System.Drawing.Size(72, 16);
            this.chk_eattend.TabIndex = 3;
            this.chk_eattend.Text = "敌方随从";
            this.chk_eattend.UseVisualStyleBackColor = true;
            // 
            // chk_sattend
            // 
            this.chk_sattend.AutoSize = true;
            this.chk_sattend.Location = new System.Drawing.Point(977, 93);
            this.chk_sattend.Name = "chk_sattend";
            this.chk_sattend.Size = new System.Drawing.Size(72, 16);
            this.chk_sattend.TabIndex = 4;
            this.chk_sattend.Text = "己方随从";
            this.chk_sattend.UseVisualStyleBackColor = true;
            // 
            // chk_eequip
            // 
            this.chk_eequip.AutoSize = true;
            this.chk_eequip.Location = new System.Drawing.Point(977, 114);
            this.chk_eequip.Name = "chk_eequip";
            this.chk_eequip.Size = new System.Drawing.Size(72, 16);
            this.chk_eequip.TabIndex = 5;
            this.chk_eequip.Text = "敌方武器";
            this.chk_eequip.UseVisualStyleBackColor = true;
            // 
            // chk_sequip
            // 
            this.chk_sequip.AutoSize = true;
            this.chk_sequip.Location = new System.Drawing.Point(977, 135);
            this.chk_sequip.Name = "chk_sequip";
            this.chk_sequip.Size = new System.Drawing.Size(72, 16);
            this.chk_sequip.TabIndex = 6;
            this.chk_sequip.Text = "己方武器";
            this.chk_sequip.UseVisualStyleBackColor = true;
            // 
            // chk_ehand
            // 
            this.chk_ehand.AutoSize = true;
            this.chk_ehand.Location = new System.Drawing.Point(977, 156);
            this.chk_ehand.Name = "chk_ehand";
            this.chk_ehand.Size = new System.Drawing.Size(72, 16);
            this.chk_ehand.TabIndex = 7;
            this.chk_ehand.Text = "敌方手牌";
            this.chk_ehand.UseVisualStyleBackColor = true;
            // 
            // chk_shand
            // 
            this.chk_shand.AutoSize = true;
            this.chk_shand.Location = new System.Drawing.Point(977, 177);
            this.chk_shand.Name = "chk_shand";
            this.chk_shand.Size = new System.Drawing.Size(72, 16);
            this.chk_shand.TabIndex = 8;
            this.chk_shand.Text = "己方手牌";
            this.chk_shand.UseVisualStyleBackColor = true;
            // 
            // txt_random
            // 
            this.txt_random.Enabled = false;
            this.txt_random.Location = new System.Drawing.Point(902, 140);
            this.txt_random.Name = "txt_random";
            this.txt_random.Size = new System.Drawing.Size(52, 21);
            this.txt_random.TabIndex = 0;
            // 
            // chk_include
            // 
            this.chk_include.AutoSize = true;
            this.chk_include.Location = new System.Drawing.Point(1076, 127);
            this.chk_include.Name = "chk_include";
            this.chk_include.Size = new System.Drawing.Size(84, 16);
            this.chk_include.TabIndex = 11;
            this.chk_include.Text = "包括施法者";
            this.chk_include.UseVisualStyleBackColor = true;
            // 
            // txt_condition
            // 
            this.txt_condition.Location = new System.Drawing.Point(1197, 123);
            this.txt_condition.Name = "txt_condition";
            this.txt_condition.Size = new System.Drawing.Size(53, 21);
            this.txt_condition.TabIndex = 12;
            // 
            // chk_useHand
            // 
            this.chk_useHand.AutoSize = true;
            this.chk_useHand.Location = new System.Drawing.Point(1257, 127);
            this.chk_useHand.Name = "chk_useHand";
            this.chk_useHand.Size = new System.Drawing.Size(96, 16);
            this.chk_useHand.TabIndex = 13;
            this.chk_useHand.Text = "使用手选目标";
            this.chk_useHand.UseVisualStyleBackColor = true;
            // 
            // txt_percent
            // 
            this.txt_percent.Location = new System.Drawing.Point(947, 261);
            this.txt_percent.Name = "txt_percent";
            this.txt_percent.Size = new System.Drawing.Size(36, 21);
            this.txt_percent.TabIndex = 15;
            // 
            // chk_IsHalo
            // 
            this.chk_IsHalo.AutoSize = true;
            this.chk_IsHalo.Location = new System.Drawing.Point(1212, 266);
            this.chk_IsHalo.Name = "chk_IsHalo";
            this.chk_IsHalo.Size = new System.Drawing.Size(72, 16);
            this.chk_IsHalo.TabIndex = 18;
            this.chk_IsHalo.Text = "光环属性";
            this.chk_IsHalo.UseVisualStyleBackColor = true;
            // 
            // txt_value1
            // 
            this.txt_value1.Location = new System.Drawing.Point(1040, 261);
            this.txt_value1.Name = "txt_value1";
            this.txt_value1.Size = new System.Drawing.Size(48, 21);
            this.txt_value1.TabIndex = 16;
            // 
            // txt_value2
            // 
            this.txt_value2.Location = new System.Drawing.Point(1129, 261);
            this.txt_value2.Name = "txt_value2";
            this.txt_value2.Size = new System.Drawing.Size(46, 21);
            this.txt_value2.TabIndex = 17;
            // 
            // txt_state
            // 
            this.txt_state.Location = new System.Drawing.Point(1326, 261);
            this.txt_state.Name = "txt_state";
            this.txt_state.Size = new System.Drawing.Size(37, 21);
            this.txt_state.TabIndex = 19;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(1291, 270);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 12);
            this.label2.TabIndex = 14;
            this.label2.Text = "状态";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(906, 270);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(35, 12);
            this.label4.TabIndex = 14;
            this.label4.Text = "成功%";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(999, 270);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(35, 12);
            this.label5.TabIndex = 14;
            this.label5.Text = "数值1";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(1088, 270);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(35, 12);
            this.label6.TabIndex = 14;
            this.label6.Text = "数值2";
            // 
            // btn_create
            // 
            this.btn_create.Location = new System.Drawing.Point(902, 315);
            this.btn_create.Name = "btn_create";
            this.btn_create.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.btn_create.Size = new System.Drawing.Size(258, 46);
            this.btn_create.TabIndex = 20;
            this.btn_create.Text = "生成";
            this.btn_create.UseVisualStyleBackColor = true;
            this.btn_create.Click += new System.EventHandler(this.btn_create_Click);
            // 
            // txt_outstr
            // 
            this.txt_outstr.Location = new System.Drawing.Point(694, 367);
            this.txt_outstr.Multiline = true;
            this.txt_outstr.Name = "txt_outstr";
            this.txt_outstr.Size = new System.Drawing.Size(659, 37);
            this.txt_outstr.TabIndex = 22;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(846, 142);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(53, 12);
            this.label7.TabIndex = 17;
            this.label7.Text = "随机数值";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(1162, 127);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(29, 12);
            this.label8.TabIndex = 18;
            this.label8.Text = "条件";
            // 
            // chk_myself
            // 
            this.chk_myself.AutoSize = true;
            this.chk_myself.Location = new System.Drawing.Point(977, 198);
            this.chk_myself.Name = "chk_myself";
            this.chk_myself.Size = new System.Drawing.Size(48, 16);
            this.chk_myself.TabIndex = 9;
            this.chk_myself.Text = "自身";
            this.chk_myself.UseVisualStyleBackColor = true;
            // 
            // chk_left_right
            // 
            this.chk_left_right.AutoSize = true;
            this.chk_left_right.Location = new System.Drawing.Point(977, 219);
            this.chk_left_right.Name = "chk_left_right";
            this.chk_left_right.Size = new System.Drawing.Size(72, 16);
            this.chk_left_right.TabIndex = 10;
            this.chk_left_right.Text = "随从两侧";
            this.chk_left_right.UseVisualStyleBackColor = true;
            this.chk_left_right.CheckedChanged += new System.EventHandler(this.chk_left_right_CheckedChanged);
            // 
            // comb_effect
            // 
            this.comb_effect.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comb_effect.Font = new System.Drawing.Font("SimSun", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.comb_effect.FormattingEnabled = true;
            this.comb_effect.IntegralHeight = false;
            this.comb_effect.ItemHeight = 16;
            this.comb_effect.Items.AddRange(new object[] {
            "1\t对目标造成X点伤害",
            "2\t对目标恢复X点生命",
            "3\t为目标抽X张牌",
            "4\t消灭目标",
            "5\t为目标增加X法力水晶",
            "6\t为目标增加X法力水晶上限",
            "7\t为目标召唤A随从X个",
            "8\t添加圣盾",
            "9\t添加冲锋",
            "10\t添加嘲讽",
            "11\t添加风怒",
            "12\t添加潜行",
            "13\t随机弃X张手牌",
            "14\t增加护甲",
            "15\t设置目标攻击为X",
            "16\t设置目标血量为X",
            "17\t为目标添加亡语ID",
            "18\t为目标添加回合开始ID（1临时，0永久）",
            "19\t为目标添加回合结束ID（1临时，0永久）",
            "20\t减少目标攻击X点",
            "21\t造成X点到Y点伤害(无法伤加成) ",
            "22\t造成X点到Y点伤害(有法伤加成)",
            "23\t消除潜行",
            "24\t造成等同于目标攻击力的伤害",
            "25\t血量乘以x",
            "26\t攻击力乘以x",
            "27\t回到拥有者手牌",
            "28\t回到己方手牌",
            "29\t回到敌方手牌",
            "30\t为目标增加X点法术伤害",
            "31\t从X库召唤Y个随从到战场",
            "32\t从X库召唤Y个卡牌到手牌",
            "33\t为目标添加敌方回合开始ID（1临时，0永久）",
            "34\t为目标添加敌方回合结束ID（1临时，0永久）",
            "35\t复制X张目标到己方手牌",
            "36\t复制X张目标到敌方手牌",
            "37\t复制X张目标到己方牌库",
            "38\t复制X张目标到敌方牌库",
            "39\t洗回目标到己方牌库",
            "40\t洗回目标到敌方牌库",
            "41\t与目标交换血量",
            "42\t目标随从攻血互换",
            "43\t将目标变为X随从",
            "44\t对目标造成等同攻击者攻击的伤害",
            "45\t将目标变为X或Y",
            "46\t目标法力值增加X",
            "47\t目标法力值减少X",
            "48\t复制X个目标随从到己方场上",
            "49\t复制X个目标随从到敌方场上",
            "50\t为目标召唤X武器"});
            this.comb_effect.Location = new System.Drawing.Point(692, 260);
            this.comb_effect.MaxDropDownItems = 10;
            this.comb_effect.Name = "comb_effect";
            this.comb_effect.Size = new System.Drawing.Size(205, 24);
            this.comb_effect.TabIndex = 14;
            // 
            // btn_reset
            // 
            this.btn_reset.Location = new System.Drawing.Point(1192, 315);
            this.btn_reset.Name = "btn_reset";
            this.btn_reset.Size = new System.Drawing.Size(107, 46);
            this.btn_reset.TabIndex = 21;
            this.btn_reset.Text = "重置";
            this.btn_reset.UseVisualStyleBackColor = true;
            this.btn_reset.Click += new System.EventHandler(this.btn_reset_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1378, 466);
            this.Controls.Add(this.btn_reset);
            this.Controls.Add(this.comb_effect);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.txt_outstr);
            this.Controls.Add(this.btn_create);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.chk_useHand);
            this.Controls.Add(this.chk_IsHalo);
            this.Controls.Add(this.chk_include);
            this.Controls.Add(this.txt_condition);
            this.Controls.Add(this.txt_state);
            this.Controls.Add(this.txt_value2);
            this.Controls.Add(this.txt_value1);
            this.Controls.Add(this.txt_percent);
            this.Controls.Add(this.txt_random);
            this.Controls.Add(this.chk_left_right);
            this.Controls.Add(this.chk_myself);
            this.Controls.Add(this.chk_shand);
            this.Controls.Add(this.chk_sequip);
            this.Controls.Add(this.chk_ehand);
            this.Controls.Add(this.chk_eequip);
            this.Controls.Add(this.chk_sattend);
            this.Controls.Add(this.chk_eattend);
            this.Controls.Add(this.chk_shero);
            this.Controls.Add(this.chk_ehero);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.btn_clearInput);
            this.Controls.Add(this.txt_output);
            this.Controls.Add(this.btn_translate);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txt_input);
            this.Name = "Form1";
            this.Text = "技能效果解释器";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox txt_input;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btn_translate;
        private System.Windows.Forms.TextBox txt_output;
        private System.Windows.Forms.Button btn_clearInput;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.RadioButton radio_4;
        private System.Windows.Forms.RadioButton radio_3;
        private System.Windows.Forms.RadioButton radio_2;
        private System.Windows.Forms.RadioButton radio_1;
        private System.Windows.Forms.CheckBox chk_ehero;
        private System.Windows.Forms.CheckBox chk_shero;
        private System.Windows.Forms.CheckBox chk_eattend;
        private System.Windows.Forms.CheckBox chk_sattend;
        private System.Windows.Forms.CheckBox chk_eequip;
        private System.Windows.Forms.CheckBox chk_sequip;
        private System.Windows.Forms.CheckBox chk_ehand;
        private System.Windows.Forms.CheckBox chk_shand;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.TextBox txt_random;
        private System.Windows.Forms.CheckBox chk_include;
        private System.Windows.Forms.TextBox txt_condition;
        private System.Windows.Forms.CheckBox chk_useHand;
        private System.Windows.Forms.TextBox txt_percent;
        private System.Windows.Forms.CheckBox chk_IsHalo;
        private System.Windows.Forms.TextBox txt_value1;
        private System.Windows.Forms.TextBox txt_value2;
        private System.Windows.Forms.TextBox txt_state;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button btn_create;
        private System.Windows.Forms.TextBox txt_outstr;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.CheckBox chk_myself;
        private System.Windows.Forms.CheckBox chk_left_right;
        private System.Windows.Forms.ComboBox comb_effect;
        private System.Windows.Forms.Button btn_reset;
    }
}

