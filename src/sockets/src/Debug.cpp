#include "Debug.h"
#include <stdarg.h>


#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


std::map<unsigned long, int> Debug::m_level;
const char *Debug::colors[] = {
		"\x1B[0;0m", // &n */
		"\x1B[0;0m\x1B[31m", // &r */
		"\x1B[0;0m\x1B[32m", // &g */
		"\x1B[0;0m\x1B[33m", // &y */
		"\x1B[0;0m\x1B[34m", // &b */
		"\x1B[0;0m\x1B[35m", // &m */
		"\x1B[0;0m\x1B[36m", // &c */
		"\x1B[0;0m\x1B[37m", // &w */
		"\x1B[0;0m\x1B[30m", // &l */
		"\x1B[1;31m", // &R */
		"\x1B[1;32m", // &G */
		"\x1B[1;33m", // &Y */
		"\x1B[1;34m", // &B */
		"\x1B[1;35m", // &M */
		"\x1B[1;36m", // &C */
		"\x1B[1;37m", // &W */
		"\x1B[1;30m" }; // &L */



void Debug::Print(const char *format, ...)
{
	char slask[5000]; // temporary for vsprintf / vsnprintf
	va_list ap;

	va_start(ap, format);
#ifdef _WIN32
	vsprintf(slask, format, ap);
#else
	vsnprintf(slask, 5000, format, ap);
#endif
	va_end(ap);

	fprintf(stderr, "%s", colors[Utility::ThreadID() % 16 + 1]);
	for (int i = 0; i < m_level[Utility::ThreadID()]; i++)
		fprintf(stderr, "  ");
	if (slask[strlen(slask) - 1] == '\n')
		slask[strlen(slask) - 1] = 0;
	fprintf(stderr, "%s%s\n", slask, colors[0]);
}


#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif
