import { defineConfig } from 'vitepress'
import { withMermaid } from 'vitepress-plugin-mermaid'
import llmstxt from 'vitepress-plugin-llms'

const rawBase = process.env.VITEPRESS_BASE
const base = rawBase
  ? rawBase.startsWith('/')
    ? rawBase.endsWith('/') ? rawBase : `${rawBase}/`
    : `/${rawBase}/`
  : '/'

const siteTitle = 'C++ 高性能指南'
const siteDescription = '可运行的 C++ 性能工程文档，覆盖学院、架构、实践手册、参考资料与研究内容。'
const siteUrl = 'https://lessup.github.io'
const canonicalBase = '/cpp-high-performance-guide/'
const currentYear = new Date().getFullYear()

function assetPath(path: string): string {
  const normalizedPath = path.replace(/^\/+/, '')
  return `${base}${normalizedPath}`
}

function canonicalAssetUrl(path: string): string {
  const normalizedPath = path.replace(/^\/+/, '')
  return new URL(`${canonicalBase.replace(/^\/+/, '')}${normalizedPath}`, `${siteUrl}/`).toString()
}

const zhAcademyItems = [
  { text: '学院概览', link: '/zh/academy/' },
  { text: '模块总览', link: '/zh/academy/module-atlas' },
  { text: '验证原则', link: '/zh/academy/validation-doctrine' },
]

const zhArchitectureItems = [
  { text: '架构概览', link: '/zh/architecture/' },
  { text: '仓库拓扑', link: '/zh/architecture/repository-topology' },
  { text: '性能方法论', link: '/zh/architecture/performance-methodology' },
]

const zhPlaybookItems = [
  { text: '实践手册概览', link: '/zh/playbook/' },
  { text: '快速开始', link: '/zh/getting-started/quickstart' },
  { text: '安装指南', link: '/zh/getting-started/installation' },
  { text: '先决条件', link: '/zh/getting-started/prerequisites' },
  { text: '学习路径', link: '/zh/guides/learning-path' },
  { text: '性能分析指南', link: '/zh/guides/profiling-guide' },
  { text: '验证与 Sanitizer', link: '/zh/guides/validation' },
  { text: '优化决策树', link: '/zh/guides/optimization-decision-tree' },
  { text: '最佳实践', link: '/zh/guides/best-practices' },
]

const zhReferenceItems = [
  { text: '参考概览', link: '/zh/reference/' },
  { text: 'API 入口', link: '/zh/reference/api-reference' },
  { text: '内存工具', link: '/zh/reference/api/memory-utils' },
  { text: 'SIMD Wrapper', link: '/zh/reference/api/simd-wrapper' },
  { text: 'Benchmark 工具', link: '/zh/reference/api/benchmark-utils' },
  { text: '常见问题', link: '/zh/reference/faq' },
  { text: '故障排查', link: '/zh/reference/troubleshooting' },
]

const zhResearchItems = [
  { text: '研究概览', link: '/zh/research/' },
  { text: '相关工作', link: '/zh/research/related-work' },
  { text: '参考资料', link: '/zh/research/references' },
  { text: '演进记录', link: '/zh/research/evolution' },
]

const zhContributingItems = [
  { text: 'AI 开发流程', link: '/zh/contributing/ai-workflow' },
]

const zhAlgorithmsItems = [
  { text: '算法概览', link: '/zh/algorithms/' },
  { text: '排序算法', link: '/zh/algorithms/sorting' },
  { text: '哈希算法', link: '/zh/algorithms/hashing' },
]

const zhDeepDivesItems = [
  { text: '深度专题概览', link: '/zh/deep-dives/' },
  { text: '内存布局', link: '/zh/deep-dives/memory-layout' },
  { text: '无锁队列', link: '/zh/deep-dives/lock-free-queue' },
  { text: 'SIMD 内部机制', link: '/zh/deep-dives/simd-internals' },
]

const zhExerciseItems = [
  { text: '练习总览', link: '/zh/exercises/' },
  { text: '内存练习', link: '/zh/exercises/module-02-memory' },
  { text: 'SIMD 练习', link: '/zh/exercises/module-04-simd' },
  { text: '并发练习', link: '/zh/exercises/module-05-concurrency' },
  { text: '解答', link: '/zh/exercises/solutions' },
]

export default withMermaid(defineConfig({
  title: siteTitle,
  description: siteDescription,
  base,
  cleanUrls: false,
  lastUpdated: true,
  mermaid: {
    startOnLoad: false,
    securityLevel: 'strict',
    flowchart: { useMaxWidth: true, htmlLabels: true },
    sequence: { useMaxWidth: true },
    gantt: { useMaxWidth: true },
    theme: 'base',
    themeVariables: {
      fontFamily: 'Inter, system-ui, sans-serif',
      fontSize: '14px',
    },
  },
  head: [
    ['link', { rel: 'icon', type: 'image/svg+xml', href: assetPath('/logo.svg') }],
    ['meta', { name: 'theme-color', content: '#3476f6' }],
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:site_name', content: siteTitle }],
    ['meta', { property: 'og:title', content: siteTitle }],
    ['meta', { property: 'og:description', content: siteDescription }],
    ['meta', { property: 'og:image', content: canonicalAssetUrl('/og-card.png') }],
    ['meta', { property: 'og:image:alt', content: 'C++ 高性能指南文档社交卡片' }],
    ['meta', { property: 'og:locale', content: 'zh_CN' }],
    ['meta', { name: 'twitter:card', content: 'summary_large_image' }],
    ['meta', { name: 'twitter:image', content: canonicalAssetUrl('/og-card.png') }],
    ['meta', { name: 'twitter:title', content: siteTitle }],
    ['meta', { name: 'twitter:description', content: siteDescription }],
    ['meta', { name: 'keywords', content: 'C++, 性能, 学院, 架构, 实践手册, 参考, 研究, SIMD, 并发, CMake, 基准测试' }],
  ],
  locales: {
    zh: {
      label: '简体中文',
      lang: 'zh-CN',
      link: '/zh/',
      title: siteTitle,
      description: siteDescription,
      themeConfig: {
        editLink: {
          pattern: 'https://github.com/LessUp/cpp-high-performance-guide/edit/master/docs/:path',
          text: '在 GitHub 上编辑此页',
        },
        footer: {
          message: '基于 MIT 许可证发布。',
          copyright: `版权所有 © ${currentYear} C++ HPC Guide 贡献者`,
        },
        nav: [
          { text: '学院', link: '/zh/academy/', activeMatch: '/zh/academy/' },
          { text: '架构', link: '/zh/architecture/', activeMatch: '/zh/architecture/' },
          { text: '算法', link: '/zh/algorithms/', activeMatch: '/zh/algorithms/' },
          { text: '实践手册', link: '/zh/playbook/', activeMatch: '/zh/playbook/|/zh/getting-started/|/zh/guides/' },
          { text: '深度专题', link: '/zh/deep-dives/', activeMatch: '/zh/deep-dives/' },
          { text: '参考', link: '/zh/reference/', activeMatch: '/zh/reference/' },
          { text: '研究', link: '/zh/research/', activeMatch: '/zh/research/' },
          { text: '贡献', link: '/zh/contributing/ai-workflow', activeMatch: '/zh/contributing/' },
        ],
        sidebar: {
          '/zh/': [
            {
              text: '概览',
              items: [
                { text: '文档入口', link: '/zh/' },
              ],
            },
            {
              text: '学院',
              items: zhAcademyItems,
            },
            {
              text: '架构',
              items: zhArchitectureItems,
            },
            {
              text: '算法',
              items: zhAlgorithmsItems,
            },
            {
              text: '实践手册',
              items: zhPlaybookItems,
            },
            {
              text: '深度专题',
              items: zhDeepDivesItems,
            },
            {
              text: '参考',
              items: zhReferenceItems,
            },
            {
              text: '研究',
              items: zhResearchItems,
            },
            {
              text: '贡献',
              items: zhContributingItems,
            },
            {
              text: '练习',
              items: zhExerciseItems,
            },
          ],
        },
        docFooter: {
          prev: '上一页',
          next: '下一页',
        },
        outline: {
          label: '本页目录',
        },
        lastUpdated: {
          text: '最后更新',
        },
        langMenuLabel: '切换语言',
        returnToTopLabel: '返回顶部',
        sidebarMenuLabel: '菜单',
        darkModeSwitchLabel: '外观',
      },
    },
  },
  themeConfig: {
    outline: [2, 3],
    search: { provider: 'local' },
    logo: {
      light: '/logo.svg',
      dark: '/logo-dark.svg',
    },
    socialLinks: [
      { icon: 'github', link: 'https://github.com/LessUp/cpp-high-performance-guide' },
    ],
  },
  vite: {
    plugins: [llmstxt()],
    build: {
      rollupOptions: {
        output: {
          manualChunks(id) {
            if (id.includes('mermaid')) return 'mermaid-lazy'
            if (id.includes('katex')) return 'katex-lazy'
          },
        },
      },
    },
  },
}))
