import { h } from 'vue'
import type { Theme } from 'vitepress'
import DefaultTheme from 'vitepress/theme'
import './style.css'
import FigureFrame from './components/FigureFrame.vue'
import MetricStrip from './components/MetricStrip.vue'
import ReferenceList from './components/ReferenceList.vue'
import SectionHero from './components/SectionHero.vue'
import LanguageRedirect from './LanguageRedirect.vue'
import LanguageSwitcher from './LanguageSwitcher.vue'

const whitepaperComponents = {
  FigureFrame,
  MetricStrip,
  ReferenceList,
  SectionHero,
} as const

export default {
  extends: DefaultTheme,
  enhanceApp(ctx) {
    DefaultTheme.enhanceApp?.(ctx)

    Object.entries(whitepaperComponents).forEach(([name, component]) => {
      ctx.app.component(name, component)
    })
  },
  Layout() {
    return h(DefaultTheme.Layout, null, {
      'layout-top': () => h(LanguageRedirect),
      'nav-bar-content-after': () => h(LanguageSwitcher),
    })
  },
} satisfies Theme
