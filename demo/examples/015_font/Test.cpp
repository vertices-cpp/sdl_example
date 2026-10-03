#include "Test.h"
#include "OGLabel.h"
#include "OGFontAtlas.h"
#include "OGFontAtlasCache.h"
#include "OGTextFieldTTF.h"
#include "OGCamera.h"
#include "OGEventDispatcher.h"

#define PATH_RES

#include "path_head.h"

USING_OG;

static std::string prePath = og::checkPath("");
static std::string text = "一 二 三 四 五 六 七 八 九 十 百 千 万 亿 零 两 双 半 数"
"多 少 上 下 左 右 前 后 东 西 南 北 中 内 外 里 边 旁 间 际 端 底 顶 头 尾 首 末 始"
"终 天 地 日 月 星 辰 云 风 雨 雪 雷 电 霜 露 雾 霞 虹 晴 阴 阳 光 影 明 暗 亮 黑 白"
"红 橙 黄 绿 青 蓝 紫 灰 粉 棕 色 空 气 水 火 山 石 土 沙 泥 尘 岩 洞 谷 峰 岭 崖 壁"
"坡 原 野 田 林 森 树 木 花 草 叶 根 枝 果 种 苗 禾 稻 麦 豆 瓜 菜 竹 松 柏 柳 杨 桃"
"李 梅 兰 菊 莲 荷 桂 枫 藤 苔 藓 菌 鸟 兽 鱼 虫 龙 虎 狮 象 豹 狼 狐 兔 鹿 马 牛 羊"
"aaa猪 狗 猫 鸡 鸭 鹅 鸽 鹰 燕 雀 鹤 鹏 蝶 蜂 蚁 蛇 龟 贝 鲸 鲨 虾 蟹 蛙 蚕 蛛 人 男 女 老 少 幼 童 孩 子 儿 女 父 母 爸 妈 哥 姐 弟 妹 爷 奶 叔 姨 舅 姑 伯 婶 侄 甥 孙 祖 亲 友 朋 伴 侣 夫 妻 婿 媳 嫂 弟 兄 姐 妹 师 生 徒 友 敌 我 你 他 她 它 谁 自 己 众 群 队 班 组 员 官 民 兵 军 警 医 护 农 工 商 学 者 作 家 诗 人 画 家 歌 手 舞 者 演 员 导 演 主 客 宾 仆 奴 王 皇 帝 后 妃 臣 将 相 侯 伯 爵 士 侠 僧 道 尼 仙 神 鬼 妖 魔 怪 精 灵 魂 魄 身 体 头 脸 面 眼 睛 耳 鼻 口 嘴 唇 舌 牙 齿 喉 颈 肩 臂 手 指 掌 拳 肘 腕 胸 背 腰 腹 肚 脐 臀 腿 膝 脚 足 趾 跟 骨 肉 血 皮 肤 发 毛 眉 须 汗 泪 唾 液 尿 屎 屁 心 肝 脾 肺 肾 胃 肠 胆 脑 髓 筋 脉 经 络 气 神 精 血 液 津 液 生 死 病 痛 伤 残 老 幼 孕 产 育 养 健 康 疾 疗 药 针 灸 刀 钳 剪 纱 布 绷 带 床 枕 被 褥 席 垫 帐 帘 窗 门 墙 屋 房 宅 院 楼 阁 亭 塔 桥 路 道 街 巷 村 城 市 镇 乡 县 省 国 州 府 都 京 郊 区 界 境 疆 域 址 场 所 处 地 点 位 置 方 向 角 落 缝 隙 孔 洞 坑 沟 渠 河 湖 海 江 川 溪 泉 井 池 塘 湾 港 滩 岛 岸 堤 坝 闸 船 舟 舰 艇 帆 桨 锚 网 钩 竿 线 绳 链 锁 钥 匙 盒 箱 包 袋 篮 桶 瓶 罐 缸 碗 盘 碟 杯 壶 锅 勺 筷 叉 刀 板 案 桌 椅 凳 床 柜 架 橱 镜 灯 烛 扇 伞 杖 笔 墨 纸 砚 书 报 刊 册 页 章 节 句 词 字 文 诗 歌 赋 曲 戏 剧 影 画 图 表 谱 乐 器 琴 弦 笛 箫 鼓 锣 钟 铃 号 角 锣 钹 笙 竽 琵 琶 筝 箫 笛 胡 琴 鼓 板 梆 子 锣 镲 唢 呐 看 听 说 读 写 画 唱 跳 跑 走 站 坐 躺 睡 醒 吃 喝 咬 嚼 吞 咽 吐 吸 呼 吹 拉 推 拖 提 抬 扛 背 抱 搂 牵 握 抓 拿 放 扔 投 掷 抛 接 打 拍 敲 击 撞 踢 踩 跳 蹦 爬 滚 翻 转 弯 停 走 进 出 来 去 回 返 离 到 达 过 经 越 穿 透 钻 藏 躲 避 让 追 赶 逃 跑 追 逐 寻 找 捡 拾 丢 失 得 获 取 给 送 买 卖 换 借 还 租 雇 用 使 做 干 劳 动 工 作 学 习 教 育 训 练 练 习 考 试 测 验 查 看 观 察 研 究 探 索 发 现 发 明 创 造 建 设 制 造 生 产 种 植 养 殖 捕 捞 采 摘 收 割 储 藏 运 输 装 卸 修 理 打 扫 洗 刷 擦 抹 剪 切 割 缝 补 织 绣 编 结 扎 捆 绑 系 解 开 关 锁 启 闭 升 降 起 落 浮 沉 飘 飞 游 泳 漂 流 滑 翔 奔 驰 行 驶 驾 驶 乘 坐 骑 牵 引 推 拉 压 挤 捏 揉 搓 搅 拌 磨 碾 筛 滤 煮 蒸 炒 炸 烤 烧 烫 冻 冷 却 热 暖 照 射 反 射 折 射 散 聚 合 分 裂 破 碎 折 断 连 接 组 装 拆 卸 修 建 筑 砌 粉 刷 涂 绘 雕 刻 塑 铸 锻 焊 接 切 削 钻 磨 抛 光 镀 染 印 复 制 打 印 扫 描 传 输 接 收 发 送 播 放 录 制 拍 摄 剪 辑 播 映 上 演 演 奏 演 唱 表 演 比 赛 竞 争 对 抗 攻 守 防 御 进 退 胜 败 输 赢 得 失 成 败 利 弊 得 失 荣 辱 福 祸 吉 凶 安 危 存 亡 生 死 聚 散 离 合 悲 欢 喜 怒 哀 乐 忧 思 恐 惊 惧 恨 爱 憎 怜 惜 珍 惜 同 情 帮 助 救 援 支 持 反 对 赞 成 批 评 表 扬 鼓 励 安 慰 劝 告 警 告 命 令 请 求 要 求 建 议 提 议 商 量 讨 论 争 论 辩 论 谈 判 签 约 合 作 分 工 交 流 沟 通 联 系 拜 访 招 待 迎 送 告 别 祝 贺 庆 祝 纪 念 悼 念 祭 祀 祈 祷 许 愿 占 卜 预 测 推 算 计 算 统 计 测 量 比 较 分 类 排 序 编 号 登 记 记 录 摘 抄 翻 译 解 释 说 明 介 绍 宣 传 广 告 通 知 报 道 采 访 调 查 研 究 分 析 综 合 归 纳 演 绎 推 理 判 断 决 定 选 择 放 弃 坚 持 坚 持 努 力 奋 斗 拼 搏 争 取 追 求 希 望 梦 想 幻 想 想 象 回 忆 思 念 怀 念 忘 记 原 谅 宽 恕 忍 耐 克 制 控 制 管 理 领 导 指 挥 组 织 协 调 配 合 服 从 遵 守 违 反 破 坏 保 护 爱 护 维 护 修 复 恢 复 改 变 改 革 改 进 提 高 降 低 增 加 减 少 扩 大 缩 小 延 长 缩 短 加 快 减 慢 提 前 推 迟 开 始 结 束 继 续 中 断 暂 停 停 止 完 成 实 现 达 成 失 败 成 功 胜 利 失 败 犯 错 改 正 惩 罚 奖 励 表 彰 批 评 检 讨 反 省 总 结 计 划 安 排 准 备 预 备 开 始 进 行 结 束 收 尾 善 后 处 理 解 决 应 对 面 对 克 服 战 胜 征 服 占 领 控 制 掌 握 运 用 利 用 使 用 消 耗 节 约 浪 费 珍 惜 爱 惜 保 护 破 坏 损 坏 修 理 维 护 保 养 清 洁 打 扫 收 拾 整 理 归 纳 摆 放 陈 列 展 示 展 览 参 观 游 览 旅 行 旅 游 度 假 休 息 休 闲 娱 乐 玩 耍 嬉 戏 运 动 锻 炼 健 身 跑 步 散 步 游 戏 下 棋 打 牌 钓 鱼 养 花 养 宠 摄 影 绘 画 书 法 写 作 阅 读 朗 诵 演 讲 辩 论 交 谈 聊 天 说 笑 幽 默 讽 刺 挖 苦 嘲 笑 赞 美 歌 颂 颂 扬 传 颂 流 传 遗 留 保 存 收 藏 珍 藏 捐 赠 捐 献 帮 助 援 助 救 济 扶 持 照 顾 护 理 陪 伴 等 待 盼 望 期 待 希 望 失 望 绝 望 悲 观 乐 观 积 极 消 极 主 动 被 动 勤 奋 懒 惰 勇 敢 怯 懦 坚 强 脆 弱 果 断 犹 豫 谨 慎 粗 心 细 心 耐 心 急 躁 冷 静 冲 动 理 智 感 性 聪 明 愚 蠢 智 慧 才 能 本 领 技 能 技 巧 方 法 办 法 措 施 手 段 途 径 渠 道 机 会 机 遇 命 运 缘 分 因 果 规 律 规 则 制 度 法 律 法 规 政 策 方 针 路 线 纲 领 计 划 方 案 方 略 策 略 战 术 战 略 目 标 任 务 使 命 责 任 义 务 权 利 权 力 利 益 好 处 坏 处 优 点 缺 点 长 处 短 处 优 势 劣 势 机 会 风 险 挑 战 困 难 问 题 矛 盾 冲 突 争 执 纠 纷 诉 讼 审 判 判 决 处 罚 赔 偿 补 偿 救 济 保 障 福 利 待 遇 工 资 薪 水 奖 金 收 入 支 出 成 本 利 润 税 收 债 务 贷 款 存 款 投 资 理 财 保 险 股 票 基 金 债 券 期 货 黄 金 外 汇 价 格 价 值 市 场 交 易 贸 易 商 业 企 业 公 司 工 厂 车 间 机 器 设 备 工 具 材 料 原 料 产 品 商 品 货 物 物 资 能 源 资 源 环 境 生 态 污 染 保 护 治 理 改 善 绿 化 美 化 净 化 节 能 减 排 可 持 续 发 展 和 谐 文 明 民 主 法 治 平 等 公 正 诚 信 友 善 爱 国 敬 业 自 由 富 强 繁 荣 昌 盛 安 定 团 结 和 平 幸 福 美 满 快 乐 开 心 高 兴 愉 悦 舒 畅 惬 意 悠 闲 自 在 潇 洒 浪 漫 温 馨 甜 蜜 苦 涩 酸 楚 辛 辣 麻 木 疼 痛 疲 劳 劳 累 轻 松 舒 适 凉 爽 温 暖 寒 冷 炎 热 干 燥 潮 湿 闷 热 凉 快 清 新 芬 芳 香 臭 甜 苦 酸 辣 咸 淡 浓 稀 稠 密 疏 紧 松 硬 软 滑 糙 粗 细 长 短 高 低 矮 胖 瘦 宽 窄 厚 薄 深 浅 大 小 多 少 快 慢 早 晚 先 后 新 旧 好 坏 美 丑 善 恶 真 假 对 错 是 非 正 邪 公 私 利 害 得 失 成 败 荣 辱 福 祸 吉 凶 安 危 存 亡 生 死 聚 散 离 合 悲 欢 喜 怒 哀 乐 忧 思 恐 惊 惧 恨 爱 憎 怜 惜 珍 惜 同 情 帮 助 救 援 支 持 反 对 赞 成 批 评 表 扬 鼓 励 安 慰 劝 告 警 告 命 令 请 求 要 求 建 议 提 议 商 量 讨 论 争 论 辩 论 谈 判 签 约 合 作 分 工 交 流 沟 通 联 系 拜 访 招 待 迎 送 告 别 祝 贺 庆 祝 纪 念 悼 念 祭 祀 祈 祷 许 愿 占 卜 预 测 推 算 计 算 统 计 测 量 比 较 分 类 排 序 编 号 登 记 记 录 摘 抄 翻 译 解 释 说 明 介 绍 宣 传 广 告 通 知 报 道 采 访 调 查 研 究 分 析 综 合 归 纳 演 绎 推 理 判 断 决 定 选 择 放 弃 坚 持";
bool Test::init()
{
	// auto label = Label::createWithTTF(text, "msyh.ttf", 24);
	auto label = Label::createWithBMFont("015_font/11.fnt", text);
	// label = Label::createWithBMFont(prePath + "015_font/11.fnt", text);
 //	label = Label::createWithCharMap(prePath + "015_font/tuffy_bold_italic-charmap.plist");
  //	label->setString("123");


	label->setMaxLineWidth(600);
	addChild(label);

//	// 方式 1：带尺寸 + 对齐
//	auto tf = TextFieldTTF::textFieldWithPlaceHolder(
//		"请输入...",                    // 占位符
//		Size(300, 40),                  // 尺寸
//		TextHAlignment::LEFT,           // 水平对齐
//		"003_n_order_bezier/msyh.ttf",                     // 字体（TTF 路径 或 系统字体名）
//		24);                            // 字号
//
//	// 方式 2：不带尺寸
//// 	auto tf = TextFieldTTF::textFieldWithPlaceHolder(
//// 		"请输入...", "msyh.ttf", 24);
//	tf->setPosition(20, 100);
//	addChild(tf);
//	tf->appendString("你好 my friend");

	auto _defaultCamera = Camera::getInstance();
	//_defaultCamera->setZoom(0.5f);   // 缩小一半
	_defaultCamera->setViewportSize(Size(300, 300));
	_defaultCamera->setViewportPos(Vec2(100, 100));
	_defaultCamera->setCenter(Vec2(300, 300));   // ← 世界原点 → 视口左上角
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesMoved = [this](const std::vector<Touch*>& touches, Event* event) {
		auto touch = touches[0];
		auto _defaultCamera = Camera::getInstance();
		auto diff = touch->getDelta();
		diff.x = (float)diff.x, diff.y = (float)diff.y;
	//	auto node = event->getCurrentTarget();// getChildByTag(1);
		auto currentPos = _defaultCamera->getCenter();
		_defaultCamera->setCenter(currentPos + diff);

	};
	_eventDispatcher->addEvent(listener);

	//   label = Label::createWithTTF(text,prePath + "003_n_order_bezier/msyh.ttf",24);
	  
	// label = Label::createWithBMFont(prePath + "015_font/11.fnt", text);
//	label = Label::createWithCharMap(prePath + "015_font/tuffy_bold_italic-charmap.plist");
 //	label->setString("123");

	
//  	label->setMaxLineWidth(500);
//   	addChild(label);
	
// 	 // label->enableOutline(Color4B::MAGENTA, 2);
// 	//label->enableOutline(Color4B::GREEN, 2);
//   	label->enableShadow(Color4B::GREEN, Vec2(1,  1));
 	//label->enableUnderline();
	scheduleUpdate();

// 	TTFConfig config(prePath + "003_n_order_bezier/msyh.ttf", 24);
// 	FontAtlas* atlas = FontAtlasCache::getFontAtlasTTF(&config);
// 	std::u32string text = U"abc中文";
// 	atlas->prepareLetterDefinitions(text);   // ★ 触发纹理 + 字形生成
// 	printf("lineHeight=%f, textures=%zu, letters=%zu\n",
// 		atlas->getLineHeight(),
// 		atlas->getTextures().size(),
// 		atlas->getLetterDefinitions().size());
// 
// 	FontAtlas* atlas1 = FontAtlasCache::getFontAtlasFNT(prePath + "015_font/11.fnt"); 
// 	printf("atlas1=%p\n", atlas1);
// 	if (atlas1)
// 	{
// 		printf("lineHeight=%f, textures=%zu, letters=%zu\n",
// 			atlas1->getLineHeight(),
// 			atlas1->getTextures().size(),
// 			atlas1->getLetterDefinitions().size());
// 	}

	return true;
}
static Color4B r[] = {
	Color4B::BLUE,
	Color4B::GRAY,
	Color4B::GREEN
};
int i = 0;
void Test::update(float dt)
{
// 	label->enableOutline(r[i], 2);
// 	i++;
// 	if (i>3)
// 	{
// 		i = 0;
// 	}
}

void Test::draw(Renderer* renderer, const Mat3& transform, uint32_t flags)
{
	Node::draw(renderer, transform, flags);

	auto camera = Camera::getVisitingCamera();
	if (!camera) return;

	// 屏幕矩形直接画（因为 draw 的 transform 是"局部 -> 屏幕"，Screen 坐标就是屏幕）
	Rect screenRect = camera->getScreenRect();

	SDL_Renderer* sdlRen = SDLView::getInstance()->getRender();
	SDL_Rect r1 = {
		(int)screenRect.origin.x,
		(int)screenRect.origin.y,
		(int)screenRect.size.width,
		(int)screenRect.size.height
	};
	SDL_Rect r2 = {
		(int)screenRect.origin.x - camera->getMarginX(),
		(int)screenRect.origin.y - camera->getMarginY(),
		(int)screenRect.size.width + camera->getMarginX() * 2,
		(int)screenRect.size.height + camera->getMarginY() * 2
	};
	SDL_SetRenderDrawColor(sdlRen, 255, 0, 0, 255);
	SDL_RenderDrawRect(sdlRen, &r1);
	SDL_RenderDrawRect(sdlRen, &r2);
}