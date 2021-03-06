#include "FileUtilsTest.h"


TESTLAYER_CREATE_FUNC(TestResolutionDirectories);
TESTLAYER_CREATE_FUNC(TestSearchPath);
TESTLAYER_CREATE_FUNC(TestFilenameLookup);

static NEWTESTFUNC createFunctions[] = {
    CF(TestResolutionDirectories),
    CF(TestSearchPath),
    CF(TestFilenameLookup)
};

static int sceneIdx=-1;
#define MAX_LAYER (sizeof(createFunctions) / sizeof(createFunctions[0]))

static CCLayer* nextAction()
{
    sceneIdx++;
    sceneIdx = sceneIdx % MAX_LAYER;
    
    CCLayer* pLayer = (createFunctions[sceneIdx])();
    pLayer->init();
    pLayer->autorelease();
    
    return pLayer;
}

static CCLayer* backAction()
{
    sceneIdx--;
    int total = MAX_LAYER;
    if( sceneIdx < 0 )
        sceneIdx += total;
    
    CCLayer* pLayer = (createFunctions[sceneIdx])();
    pLayer->init();
    pLayer->autorelease();
    
    return pLayer;
}

static CCLayer* restartAction()
{
    CCLayer* pLayer = (createFunctions[sceneIdx])();
    pLayer->init();
    pLayer->autorelease();
    
    return pLayer;
}

void FileUtilsTestScene::runThisTest()
{
    CCLayer* pLayer = nextAction();
    addChild(pLayer);
    
    CCDirector::sharedDirector()->replaceScene(this);
}

// #pragma mark - FileUtilsDemo

void FileUtilsDemo::onEnter()
{
    CCLayer::onEnter();
    
    CCLabelTTF* label = CCLabelTTF::create(title().c_str(), "Arial", 32);
    addChild(label);
    label->setPosition(ccp(VisibleRect::center().x, VisibleRect::top().y-50));
    
    std::string subTitle = subtitle();
    if(! subTitle.empty())
    {
        CCLabelTTF* l = CCLabelTTF::create(subTitle.c_str(), "Thonburi", 16);
        addChild(l, 1);
        l->setPosition(ccp(VisibleRect::center().x, VisibleRect::top().y-80));
    }
    
    CCMenuItemImage *item1 = CCMenuItemImage::create("Images/b1.png", "Images/b2.png", this, menu_selector(FileUtilsDemo::backCallback));
    CCMenuItemImage *item2 = CCMenuItemImage::create("Images/r1.png","Images/r2.png", this, menu_selector(FileUtilsDemo::restartCallback) );
    CCMenuItemImage *item3 = CCMenuItemImage::create("Images/f1.png", "Images/f2.png", this, menu_selector(FileUtilsDemo::nextCallback) );
    
    CCMenu *menu = CCMenu::create(item1, item2, item3, NULL);
    menu->setPosition(CCPointZero);
    item1->setPosition(ccp(VisibleRect::center().x - item2->getContentSize().width*2, VisibleRect::bottom().y+item2->getContentSize().height/2));
    item2->setPosition(ccp(VisibleRect::center().x, VisibleRect::bottom().y+item2->getContentSize().height/2));
    item3->setPosition(ccp(VisibleRect::center().x + item2->getContentSize().width*2, VisibleRect::bottom().y+item2->getContentSize().height/2));
    
    addChild(menu, 1);
}

void FileUtilsDemo::backCallback(CCObject* pSender)
{
    CCScene* pScene = new FileUtilsTestScene();
    CCLayer* pLayer = backAction();
    
    pScene->addChild(pLayer);
    CCDirector::sharedDirector()->replaceScene(pScene);
    pScene->release();
}

void FileUtilsDemo::nextCallback(CCObject* pSender)
{
    CCScene* pScene = new FileUtilsTestScene();
    CCLayer* pLayer = nextAction();
    
    pScene->addChild(pLayer);
    CCDirector::sharedDirector()->replaceScene(pScene);
    pScene->release();
}

void FileUtilsDemo::restartCallback(CCObject* pSender)
{
    CCScene* pScene = new FileUtilsTestScene();
    CCLayer* pLayer = restartAction();
    
    pScene->addChild(pLayer);
    CCDirector::sharedDirector()->replaceScene(pScene);
    pScene->release();
}

string FileUtilsDemo::title()
{
    return "No title";
}

string FileUtilsDemo::subtitle()
{
    return "";
}

//#pragma mark - TestResolutionDirectories

void TestResolutionDirectories::onEnter()
{
    FileUtilsDemo::onEnter();
    CCFileUtils *sharedFileUtils = CCFileUtils::sharedFileUtils();

    string ret;
    
    sharedFileUtils->purgeCachedEntries();
    m_defaultSearchPathArray = sharedFileUtils->getSearchPaths();
    vector<string> searchPaths = m_defaultSearchPathArray;
    searchPaths.insert(searchPaths.begin(),   "Misc");
    sharedFileUtils->setSearchPaths(searchPaths);
    
    m_defaultResolutionsOrderArray = sharedFileUtils->getSearchResolutionsOrder();
    vector<string> resolutionsOrder = m_defaultResolutionsOrderArray;

    resolutionsOrder.insert(resolutionsOrder.begin(), "resources-ipadhd");
    resolutionsOrder.insert(resolutionsOrder.begin()+1, "resources-ipad");
    resolutionsOrder.insert(resolutionsOrder.begin()+2, "resources-widehd");
    resolutionsOrder.insert(resolutionsOrder.begin()+3, "resources-wide");
    resolutionsOrder.insert(resolutionsOrder.begin()+4, "resources-hd");
    resolutionsOrder.insert(resolutionsOrder.begin()+5, "resources-iphone");
    
    sharedFileUtils->setSearchResolutionsOrder(resolutionsOrder);
    
    for( int i=1; i<7; i++) {
        CCString *filename = CCString::createWithFormat("test%d.txt", i);
        ret = sharedFileUtils->fullPathForFilename(filename->getCString());
        CCLog("%s -> %s", filename->getCString(), ret.c_str());
    }
}

void TestResolutionDirectories::onExit()
{
    CCFileUtils *sharedFileUtils = CCFileUtils::sharedFileUtils();
    
	// reset search path
	sharedFileUtils->setSearchPaths(m_defaultSearchPathArray);
    sharedFileUtils->setSearchResolutionsOrder(m_defaultResolutionsOrderArray);
    FileUtilsDemo::onExit();
}

string TestResolutionDirectories::title()
{
    return "FileUtils: resolutions in directories";
}

string TestResolutionDirectories::subtitle()
{
    return "See the console";
}

//#pragma mark - TestSearchPath

void TestSearchPath::onEnter()
{
    FileUtilsDemo::onEnter();
    CCFileUtils *sharedFileUtils = CCFileUtils::sharedFileUtils();
    
    string ret;
    
    sharedFileUtils->purgeCachedEntries();
    m_defaultSearchPathArray = sharedFileUtils->getSearchPaths();
    vector<string> searchPaths = m_defaultSearchPathArray;
    string writablePath = sharedFileUtils->getWriteablePath();
    string fileName = writablePath+"external.txt";
    char szBuf[100] = "Hello Cocos2d-x!";
    FILE* fp = fopen(fileName.c_str(), "wb");
    if (fp)
    {
        fwrite(szBuf, 1, strlen(szBuf), fp);
        fclose(fp);
        CCLog("Writing file to writable path succeed.");
    }
    
    searchPaths.insert(searchPaths.begin(), writablePath);
    searchPaths.insert(searchPaths.begin()+1,   "Misc/searchpath1");
    searchPaths.insert(searchPaths.begin()+2, "Misc/searchpath2");
    sharedFileUtils->setSearchPaths(searchPaths);
    
    m_defaultResolutionsOrderArray = sharedFileUtils->getSearchResolutionsOrder();
    vector<string> resolutionsOrder = m_defaultResolutionsOrderArray;
    
    resolutionsOrder.insert(resolutionsOrder.begin(), "resources-ipad");
    sharedFileUtils->setSearchResolutionsOrder(resolutionsOrder);
    
    for( int i=1; i<3; i++) {
        CCString *filename = CCString::createWithFormat("file%d.txt", i);
        ret = sharedFileUtils->fullPathForFilename(filename->getCString());
        CCLog("%s -> %s", filename->getCString(), ret.c_str());
    }
    
    // Gets external.txt from writable path
    string fullPath = sharedFileUtils->fullPathForFilename("external.txt");
    CCLog("\nexternal file path = %s\n", fullPath.c_str());
    if (fullPath.length() > 0) {
        fp = fopen(fullPath.c_str(), "rb");
        if (fp)
        {
            char szReadBuf[100] = {0};
            fread(szReadBuf, 1, strlen(szBuf), fp);
            CCLog("The content of file from writable path: %s", szReadBuf);
            fclose(fp);
        }
    }
}

void TestSearchPath::onExit()
{
	CCFileUtils *sharedFileUtils = CCFileUtils::sharedFileUtils();

	// reset search path
	sharedFileUtils->setSearchPaths(m_defaultSearchPathArray);
    sharedFileUtils->setSearchResolutionsOrder(m_defaultResolutionsOrderArray);
    FileUtilsDemo::onExit();
}

string TestSearchPath::title()
{
    return "FileUtils: search path";
}

string TestSearchPath::subtitle()
{
    return "See the console";
}

//#pragma mark - TestFilenameLookup

void TestFilenameLookup::onEnter()
{
    FileUtilsDemo::onEnter();
		
    CCFileUtils *sharedFileUtils = CCFileUtils::sharedFileUtils();

    CCDictionary *dict = CCDictionary::create();
    dict->setObject(CCString::create("Images/grossini.png"), "grossini.bmp");
    dict->setObject(CCString::create("Images/grossini.png"), "grossini.xcf");
    
    sharedFileUtils->setFilenameLookupDictionary(dict);
    
    
    // Instead of loading carlitos.xcf, it will load grossini.png
    CCSprite *sprite = CCSprite::create("grossini.xcf");
    this->addChild(sprite);
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    sprite->setPosition(ccp(s.width/2, s.height/2));
}

void TestFilenameLookup::onExit()
{
	
	CCFileUtils *sharedFileUtils = CCFileUtils::sharedFileUtils();
	
	// reset filename lookup
    sharedFileUtils->setFilenameLookupDictionary(CCDictionary::create());
	
    FileUtilsDemo::onExit();
}

string TestFilenameLookup::title()
{
    return "FileUtils: filename lookup";
}

string TestFilenameLookup::subtitle()
{
    return "See the console";
}

