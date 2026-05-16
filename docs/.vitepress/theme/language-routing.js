export const SUPPORTED_LANGS = [
  { code: 'en', label: 'English', path: '/en/' },
  { code: 'zh', label: '中文', path: '/zh/' },
]

function ensureLeadingSlash(path) {
  return path.startsWith('/') ? path : `/${path}`
}

export function normalizeSiteBase(siteBase = '/') {
  return siteBase.endsWith('/') ? siteBase : `${siteBase}/`
}

export function stripBase(path, siteBase = '/') {
  const normalizedPath = ensureLeadingSlash(path)
  const normalizedBase = normalizeSiteBase(siteBase)

  if (normalizedBase === '/') {
    return normalizedPath
  }

  const baseVariants = [...new Set([normalizedBase, normalizedBase.replace(/\/$/, '')])]

  for (const baseVariant of baseVariants) {
    if (!baseVariant) {
      continue
    }

    if (normalizedPath === baseVariant || normalizedPath === `${baseVariant}/`) {
      return '/'
    }

    if (normalizedPath.startsWith(`${baseVariant}/`)) {
      const stripped = normalizedPath.slice(baseVariant.length).replace(/^\/+/, '')
      return stripped ? `/${stripped}` : '/'
    }
  }

  return normalizedPath
}

export function stripLocale(path, supportedLangs = SUPPORTED_LANGS) {
  for (const { path: localePath } of supportedLangs) {
    if (path === localePath || path === localePath.slice(0, -1)) {
      return '/'
    }

    if (path.startsWith(localePath)) {
      const stripped = path.slice(localePath.length)
      return stripped ? `/${stripped.replace(/^\/+/, '')}` : '/'
    }
  }

  return path
}

export function withBase(path, siteBase = '/') {
  const normalizedBase = normalizeSiteBase(siteBase)

  if (normalizedBase === '/') {
    return ensureLeadingSlash(path)
  }

  return `${normalizedBase}${path.replace(/^\/+/, '')}`
}

export function resolveLanguageTarget({
  routePath,
  siteBase = '/',
  targetLangPath,
  supportedLangs = SUPPORTED_LANGS,
}) {
  const currentPath = stripBase(routePath, siteBase)
  const pathWithoutLang = stripLocale(currentPath, supportedLangs)
  const nextPath = pathWithoutLang === '/'
    ? targetLangPath
    : `${targetLangPath}${pathWithoutLang.replace(/^\/+/, '')}`

  return withBase(nextPath, siteBase)
}
