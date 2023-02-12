#ifdef USE_SDL

#include "bitmap.h"
#include "../bitmap.h"

bool operator==(const INTERNAL_COLOR&, const INTERNAL_COLOR&){
    /* FIXME */
    return false;
}

bool operator!=(const INTERNAL_COLOR& left, const INTERNAL_COLOR& right){
    return !(left == right);
}

bool operator<(const INTERNAL_COLOR&, const INTERNAL_COLOR&){
    /* FIXME */
    return false;
}

Graphics::Bitmap::Bitmap(){
}

Graphics::Bitmap::Bitmap(int x, int y){
}

Graphics::Bitmap::Bitmap( const Bitmap & copy, bool deep_copy ){
}

Graphics::Bitmap::Bitmap( const Bitmap & copy, int x, int y, int width, int height ){
}

Graphics::Bitmap::Bitmap(const char * data, int length){
}

Graphics::Bitmap Graphics::Bitmap::createMemoryBitmap(int width, int height){
    return Graphics::Bitmap();
}

Graphics::Bitmap * Graphics::getScreenBuffer(){
    /* FIXME */
    return nullptr;
}

Graphics::Color Graphics::makeColor(int r, int g, int b){
    INTERNAL_COLOR c;
    return Graphics::Color(c);
}

void Graphics::Bitmap::fill(Graphics::Color color) const {
}

int Graphics::Bitmap::getWidth() const {
    return 0;
}

int Graphics::Bitmap::getHeight() const {
    return 0;
}

void Graphics::Bitmap::draw(const int x, const int y, Filter * filter, const Bitmap & where) const {
}

void Graphics::TranslucentBitmap::draw(const int x, const int y, const Graphics::Bitmap & where) const {
}

void Graphics::Bitmap::transBlender( int r, int g, int b, int a ){
}

void Graphics::Bitmap::loadFromMemory(const char * data, int length){
}

void Graphics::Bitmap::internalLoadFile( const char * load_file ){
}

void Graphics::Bitmap::drawHFlip(const int x, const int y, Filter * filter, const Bitmap & where) const {
}

int Graphics::getRed(Color x){
    return 0;
}

int Graphics::getBlue(Color x){
    return 0;
}

int Graphics::getGreen(Color x){
    return 0;
}

int Graphics::getAlpha(Color x){
    return 0;
}

Graphics::Color Graphics::Bitmap::getPixel( const int x, const int y ) const {
    Graphics::Color c;
    return c;
}

void Graphics::Bitmap::putPixel( int x, int y, Color col ) const {
}

void Graphics::Bitmap::getClipRect( int & x1, int & y1, int & x2, int & y2 ) const {
}

void Graphics::Bitmap::setClipRect( int x1, int y1, int x2, int y2 ) const {
}

Graphics::Color Graphics::MaskColor(){
    Graphics::Color c;
    return c;
}

void Graphics::initializeExtraStuff(){
}

Graphics::Bitmap::Bitmap( const std::string & load_file ){
}

void Graphics::Bitmap::applyTrans(const Color color) const {
}

Graphics::Bitmap Graphics::Bitmap::subBitmap(int x, int y, int width, int height){
    return Graphics::Bitmap();
}

void Graphics::Bitmap::save( const std::string & str ) const {
}

Graphics::Color Graphics::Bitmap::blendColor(const Color & input) const {
    return input;
}

void Graphics::Bitmap::startDrawing() const {
}

void Graphics::Bitmap::endDrawing() const {
}

void Graphics::Bitmap::triangle( int x1, int y1, int x2, int y2, int x3, int y3, Color color ) const {
}

void Graphics::Bitmap::ellipse( int x, int y, int rx, int ry, Color color ) const {
}

void Graphics::Bitmap::ellipseFill( int x, int y, int rx, int ry, Color color ) const {
}

void Graphics::Bitmap::light(int x, int y, int width, int height, int start_y, int focus_alpha, int edge_alpha, Color focus_color, Color edge_color) const {
}

void Graphics::Bitmap::rectangle( int x1, int y1, int x2, int y2, Color color ) const {
}

void Graphics::Bitmap::rectangleFill( int x1, int y1, int x2, int y2, Color color ) const {
}

void Graphics::Bitmap::circleFill( int x, int y, int radius, Color color ) const {
}

void Graphics::Bitmap::circle( int x, int y, int radius, Color color ) const {
}

void Graphics::Bitmap::circle(int x, int y, int radius, int thickness, Color color) const {
}

void Graphics::Bitmap::line( const int x1, const int y1, const int x2, const int y2, const Color color ) const {
}

void Graphics::Bitmap::floodfill( const int x, const int y, const Color color ) const {
}

void Graphics::Bitmap::hLine( const int x1, const int y, const int x2, const Color color ) const {
}

void Graphics::Bitmap::vLine( const int y1, const int x, const int y2, const Color color ) const {
}

void Graphics::Bitmap::polygon( const int * verts, const int nverts, const Color color ) const {
}

void Graphics::Bitmap::arc(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color ) const {
}

void Graphics::Bitmap::arcFilled(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color ) const {
}

void Graphics::Bitmap::draw(const int x, const int y, const Bitmap & where) const {
}

void Graphics::Bitmap::drawCharacter( const int x, const int y, const Color color, const int background, const Bitmap & where ) const {
}

void Graphics::Bitmap::drawHFlip(const int x, const int y, const Bitmap & where) const {
}

void Graphics::Bitmap::drawVFlip( const int x, const int y, const Bitmap & where ) const {
}

void Graphics::Bitmap::drawVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::Bitmap::drawHVFlip( const int x, const int y, const Bitmap & where ) const {
}

void Graphics::Bitmap::drawHVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::Bitmap::drawStretched( const int x, const int y, const int new_width, const int new_height, const Bitmap & who ) const {
}

void Graphics::Bitmap::drawRotate(const int x, const int y, const int angle, const Bitmap & where){
}

void Graphics::Bitmap::drawPivot( const int centerX, const int centerY, const int x, const int y, const int angle, const Bitmap & where ){
}

void Graphics::Bitmap::drawPivot( const int centerX, const int centerY, const int x, const int y, const int angle, const double scale, const Bitmap & where ){
}

void Graphics::Bitmap::StretchHqx(const Bitmap & where, const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight) const {
}

void Graphics::Bitmap::Stretch(const Bitmap & where, const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight ) const {
}

void Graphics::Bitmap::StretchBy2( const Bitmap & where ){
}

void Graphics::Bitmap::StretchBy4( const Bitmap & where ){
}

void Graphics::Bitmap::StretchXbr(const Bitmap & where, const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight) const {
}

void Graphics::Bitmap::Blit( const int mx, const int my, const int width, const int height, const int wx, const int wy, const Bitmap & where ) const {
}

void Graphics::Bitmap::BlitMasked( const int mx, const int my, const int width, const int height, const int wx, const int wy, const Bitmap & where ) const {
}

void Graphics::Bitmap::BlitAreaToScreen(const int upper_left_x, const int upper_left_y) const {
}

void Graphics::Bitmap::BlitToScreen(const int upper_left_x, const int upper_left_y) const {
}

void Graphics::Bitmap::roundRect(int radius, int x1, int y1, int x2, int y2, Color color) const {
}

void Graphics::Bitmap::roundRectFill(int radius, int x1, int y1, int x2, int y2, Graphics::Color color) const {
}

void Graphics::Bitmap::drawShadow(Bitmap & where, int x, int y, int intensity, Color color, double scale, bool facingRight) const {
}

Graphics::Bitmap Graphics::Bitmap::scaleTo(const int width, const int height) const {
    return Graphics::Bitmap();
}

void Graphics::Bitmap::readLine( std::vector<Color> & vec, int y ){
}

void Graphics::Bitmap::putPixelNormal(int x, int y, Color col) const {
}

Graphics::Color Graphics::TranslucentBitmap::blendColor(const Color & color) const {
    return color;
}

void Graphics::TranslucentBitmap::startDrawing() const {
}

void Graphics::TranslucentBitmap::endDrawing() const {
}

void Graphics::TranslucentBitmap::ellipse( int x, int y, int rx, int ry, Color color ) const {
}

void Graphics::TranslucentBitmap::ellipseFill( int x, int y, int rx, int ry, Color color ) const {
}

void Graphics::TranslucentBitmap::roundRect(int radius, int x1, int y1, int x2, int y2, Color color) const {
}

void Graphics::TranslucentBitmap::roundRectFill(int radius, int x1, int y1, int x2, int y2, Graphics::Color color) const {
}

void Graphics::TranslucentBitmap::circleFill( int x, int y, int radius, Color color ) const {
}

void Graphics::TranslucentBitmap::hLine( const int x1, const int y, const int x2, const Color color ) const {
}

void Graphics::TranslucentBitmap::rectangleFill(int x1, int y1, int x2, int y2, Color color) const {
}

void Graphics::TranslucentBitmap::rectangle(int x1, int y1, int x2, int y2, Color color) const {
}

void Graphics::TranslucentBitmap::line( const int x1, const int y1, const int x2, const int y2, const Color color ) const {
}

void Graphics::TranslucentBitmap::arc(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color) const {
}

void Graphics::TranslucentBitmap::arcFilled(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color ) const {
}

void Graphics::TranslucentBitmap::draw(const int x, const int y, Filter * filter, const Bitmap & where) const {
}

void Graphics::TranslucentBitmap::drawHFlip(const int x, const int y, Filter * filter, const Bitmap & where) const {
}
    
void Graphics::TranslucentBitmap::drawHFlip(const int x, const int y, const Bitmap & where) const {
}

void Graphics::TranslucentBitmap::drawVFlip( const int x, const int y, const Bitmap & where ) const {
}

void Graphics::TranslucentBitmap::drawVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::TranslucentBitmap::putPixelNormal(int x, int y, Color col) const {
}

void Graphics::TranslucentBitmap::drawHVFlip( const int x, const int y, const Bitmap & where ) const {
}

void Graphics::TranslucentBitmap::drawHVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::LitBitmap::draw(const int x, const int y, const Bitmap & where) const {
}

void Graphics::LitBitmap::draw(const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::LitBitmap::drawHFlip( const int x, const int y, const Bitmap & where ) const {
}

void Graphics::LitBitmap::drawHFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::LitBitmap::drawVFlip( const int x, const int y, const Bitmap & where ) const {
}

void Graphics::LitBitmap::drawVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

void Graphics::LitBitmap::drawHVFlip( const int x, const int y, const Bitmap & where ) const {
}

void Graphics::LitBitmap::drawHVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const {
}

int Graphics::setGfxModeText(){
    return 0;
}

int Graphics::setGraphicsMode(int mode, int width, int height){
    return 0;
}

Graphics::StretchedBitmap::StretchedBitmap(int width, int height, const Bitmap & where, Clear clear, QualityFilter filter):
where(where),
filter(filter),
clearKind(clear){
}

void Graphics::StretchedBitmap::finish(){
}

void Graphics::StretchedBitmap::start(){
}

int Graphics::changeGraphicsMode(int mode, int width, int height){
    return 0;
}

bool Graphics::Bitmap::getError(){
    /* TODO */
    return false;
}

void Graphics::Bitmap::shutdown(){
}

Graphics::RestoreState::RestoreState(){
}

Graphics::RestoreState::~RestoreState(){
}

Graphics::TranslatedBitmap::TranslatedBitmap(int x, int y, const Bitmap & where){
}

Graphics::TranslatedBitmap::~TranslatedBitmap(){
}
    
void Graphics::TranslatedBitmap::BlitToScreen() const {
}

void Graphics::Bitmap::lock() const {
}

void Graphics::Bitmap::lock(int x, int y, int width, int height) const {
}

void Graphics::Bitmap::unlock() const {
}

Graphics::Bitmap Graphics::memoryPCX(unsigned char * const data, const int length, const bool mask){
    return Graphics::Bitmap();
}

void Graphics::Bitmap::replaceColor(const Color & original, const Color & replaced){
}

void Graphics::Bitmap::addBlender( int r, int g, int b, int a ){
}

void Graphics::Bitmap::differenceBlender( int r, int g, int b, int a ){
}

void Graphics::Bitmap::alphaBlender(int source, int dest){
}

#endif
